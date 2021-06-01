#include "config.h"

key_t key;
int sem_id;
int shm_id;


void handler_SIGINT(int sig_no, siginfo_t* info, void* ucontext)
{
    semctl(sem_id, 0, IPC_RMID, NULL);
    shmctl(shm_id, IPC_RMID, NULL);
    exit(0);
}


void handle_error(char* msg){
	perror(msg);
	raise(SIGINT);
}


int main(int argc, char** argv){
	
	// EXIT SETUP	
	struct sigaction action;
    action.sa_sigaction = handler_SIGINT;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
	
	srand(getpid());
	key = ftok(PROJ_PATH, PROJ_ID);
	
	if((sem_id = semget(key, 2, 0)) == -1)
        handle_error("Failed to create semaphores!\n");
    
	int shm_id = shmget(key, sizeof(Pizzeria), 0);

	struct sembuf take_pizza = {TABLE_SEM, 1, 0};
	struct sembuf before[1] = {take_pizza};
	
	int pizza_type, waiting_pizzas;
	
	while (1){
						
		Pizzeria *pizzeria = shmat(shm_id, NULL, 0);
		
		int i = 0;
		for(; i < TABLE_CAPACITY; i++)
			if(pizzeria->table[i] != 0){
				pizza_type = pizzeria->table[i];
				pizzeria->table[i] = 0;
				break;
			}
		if(i == TABLE_CAPACITY){
			sleep(1);
			continue;
		}
		
		semop(sem_id, before, 1);
		waiting_pizzas = TABLE_CAPACITY - semctl(sem_id, TABLE_SEM, GETVAL);
		printf("(%d %lu) Pobieram pizze %d. Pizz na stole %d\n", getpid(), time(NULL), pizza_type, waiting_pizzas);
		
		sleep(4);

		printf("(%d %lu) Dostarczam pizze: %d.\n",
		 getpid(), time(NULL), pizza_type);
		
		shmdt(pizzeria);
		
		sleep(4);
	}

}
