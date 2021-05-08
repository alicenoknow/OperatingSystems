#include "config.h"

key_t key;
int sem_id;
int shm_id;
pid_t workers[COOKS_NO+DELIVERERS_NO];


void clean_workspace(){
	semctl(sem_id, 0, IPC_RMID, NULL);
    shmctl(shm_id, IPC_RMID, NULL);
}


void handler_SIGINT(int sig_no, siginfo_t* info, void* ucontext)
{
    for(int i=0; i < (COOKS_NO+DELIVERERS_NO); i++){
		if(workers[i] != 0)
			kill(workers[i], SIGINT);
    }
    exit(0);
}


void handle_error(char* msg){
	perror(msg);
	raise(SIGINT);
}


int main(int argc, char** argv){
	
	// EXIT SETUP
	atexit(clean_workspace);
	
	struct sigaction action;
    action.sa_sigaction = handler_SIGINT;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
		
		
	// RESOURCES INIT
	key = ftok(PROJ_PATH, PROJ_ID);
	if((sem_id = semget(key, 2, IPC_CREAT | IPC_EXCL| 0666)) == -1)
        handle_error("Failed to create semaphores!\n");
	
	union semun args;
	args.val = OVEN_CAPACITY;
    if(semctl(sem_id, OVEN_SEM, SETVAL, args) == -1) 
		handle_error("Error while initialising oven semaphore!\n");
		 
    args.val = TABLE_CAPACITY;
    if(semctl(sem_id, TABLE_SEM, SETVAL, args) == -1) 
		handle_error("Error while initialising table semaphore!\n");

	if((shm_id = shmget(key, sizeof(Pizzeria), IPC_CREAT | IPC_EXCL | 0666)) == -1)
        handle_error("Error while creating shared memory segment!\n");

	
	// PIZZERIA INIT
	Pizzeria* pizzeria = shmat(shm_id, NULL, 0);
	pizzeria->oven_occupied = 0;
	pizzeria->table_occupied = 0;
	
    for(int i=0; i<TABLE_CAPACITY; i++)
		pizzeria->table[i] = 0;
    for(int i=0; i<OVEN_CAPACITY; i++) 
		pizzeria->oven[i] = 0;
		
    shmdt(pizzeria);
    
    
	// WORKERS INIT
	int idx = 0;
	for(int i=0; i < COOKS_NO; i++){
        pid_t child = fork();
        if(child == 0){
            execl("./cook", "./cook", NULL);
            exit(0);
        }
        workers[idx] = child;
        idx++;
    }
    
    for(int i=0; i < DELIVERERS_NO; i++){
        pid_t child = fork();
        if(child == 0){
            execl("./deliverer", "./deliverer", NULL);
            exit(0);
        }
        workers[idx] = child;
        idx++;
        printf("Elo\n");
    }
    
    for(int i = 0 ; i < COOKS_NO+DELIVERERS_NO; i++){
        wait(NULL);
	}
	
	return 0;
}
