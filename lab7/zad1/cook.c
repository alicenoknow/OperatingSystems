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

	struct sembuf put_pizza = {OVEN_SEM, -1, 0};
	struct sembuf before[1] = {put_pizza};
	
	struct sembuf take_pizza = {OVEN_SEM, 1, 0};
	struct sembuf put_on_table = {TABLE_SEM, -1, 0};
	struct sembuf after[2] = {take_pizza, put_on_table};
	
	int pizza_type, baking_pizzas, waiting_pizzas, idx;
	
	while (1){
		
		pizza_type = rand() % PIZZA_TYPES + 1;
		
		printf("(%d %lu) Przygotowuje pizze %d. \n", getpid(), time(NULL), pizza_type);
		sleep(2);
		
		semop(sem_id, before, 1);
		Pizzeria *pizzeria = shmat(shm_id, NULL, 0);
		
		for(int i=0; i < OVEN_CAPACITY; i++)
			if(pizzeria->oven[i] == 0){
				pizzeria->oven[i] = pizza_type;
				idx = i;
				break;
			}

		baking_pizzas = OVEN_CAPACITY - semctl(sem_id, OVEN_SEM, GETVAL);
		printf("(%d %lu) Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), time(NULL), pizza_type, baking_pizzas);
		
		sleep(4);
		
		pizzeria->oven[idx] = 0;
		semop(sem_id, after, 2);
		
		for(int i=0; i < TABLE_CAPACITY; i++)
			if(pizzeria->table[i] == 0){
				pizzeria->table[i] = pizza_type;
				break;
			}
		
		baking_pizzas = OVEN_CAPACITY - semctl(sem_id, OVEN_SEM, GETVAL);
		waiting_pizzas = TABLE_CAPACITY - semctl(sem_id, TABLE_SEM, GETVAL);

		printf("(%d %lu) Wyjmuję pizze: %d. Liczba pizz w piecu: %d Liczba pizz na stole: %d.\n",
		 getpid(), time(NULL), pizza_type, baking_pizzas, waiting_pizzas);
		
		shmdt(pizzeria);
	}

}
