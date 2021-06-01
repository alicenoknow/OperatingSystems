#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>


#define REINDEERS 9
#define ELVES 5
#define ELVES_WAIT_MAX 3
#define WORK_TIME rand() % 5 + 2
#define HOLIDAY_TIME rand() % 9 + 2
#define PROBLEM_TIME rand() % 3 + 1
#define GIFT_TIME rand() % 3 + 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeers_mutex = PTHREAD_MUTEX_INITIALIZER;

int gift_delivery = 0;
int waiting_elves_no = 0;
int waiting_elves[ELVES];
int waiting_reindeers_no = 0;
int delivering = 0;

pthread_cond_t elves_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeers_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;


void* santa(void* args){

	while (gift_delivery < 3) {
		pthread_mutex_lock(&mutex);

		while (waiting_elves_no < ELVES_WAIT_MAX && waiting_reindeers_no < REINDEERS) {
			printf("Santa: sleeping, wainting elves: %d, waiting reindeers %d\n", waiting_elves_no, waiting_reindeers_no);
            pthread_cond_wait(&santa_cond, &mutex);
		}

		printf("Santa: waking up, elves: %d, reinideers %d\n", waiting_elves_no, waiting_reindeers_no);

		if (waiting_reindeers_no == REINDEERS) {
			gift_delivery++;
            printf("Santa:  delivering gifts [delivery no: %d]\n", gift_delivery);
            sleep(GIFT_TIME);
            waiting_reindeers_no = 0;
			pthread_cond_broadcast(&reindeers_cond);
		}
		else if (waiting_elves_no == ELVES_WAIT_MAX) {
            printf("Santa: solvng elves problems: %d, %d, %d\n", waiting_elves[0], waiting_elves[1], waiting_elves[2]);
            sleep(PROBLEM_TIME);
			waiting_elves_no = 0;

            pthread_cond_broadcast(&elves_cond);
        }
		pthread_mutex_unlock(&mutex);
	}
}


void* reindeer(void* args){
	int id = *((int *) args);
  	while (1) {
    	sleep(HOLIDAY_TIME);
		pthread_mutex_lock(&mutex);

		while (waiting_reindeers_no == REINDEERS)
            pthread_cond_wait(&reindeers_cond, &mutex);

		waiting_reindeers_no++;
		printf("Reindeer commng back, id: %d \n", id);

		if (waiting_reindeers_no == REINDEERS) {
			printf("Reindeers: all reindeers came, waking santa up, ID: %d\n", id);
			pthread_cond_broadcast(&santa_cond);
		}

		pthread_mutex_unlock(&mutex);
			
  	}
  	return NULL;

}

void* elf (void* args) {
	int id = *((int *) args);
  	while (1) {
    	sleep(WORK_TIME);
		pthread_mutex_lock(&mutex);

		while (waiting_elves_no == ELVES_WAIT_MAX) {
			printf("Elf: waiting..., ID: %d\n", id);
            pthread_cond_wait(&elves_cond, &mutex);
		}

		printf("Elf has problem, id: %d \n", id);
		waiting_elves[waiting_elves_no] = id;
		waiting_elves_no++;

		if (waiting_elves_no == ELVES_WAIT_MAX) {
			printf("Elf: third elf came, waking santa up, ID: %d\n", id);
			pthread_cond_broadcast(&santa_cond);
		}

		pthread_mutex_unlock(&mutex);
  	}
  	return NULL;
}


int main(){
	srand(time(NULL));

    // Santa 
    pthread_t santa_thread;
    pthread_create(&santa_thread, NULL, &santa, NULL);

    // Reindeers
    int* reindeers_id = calloc(REINDEERS, sizeof(int));
    pthread_t* reindeers_threads = calloc(REINDEERS, sizeof(pthread_t));
    for (int i = 0; i < REINDEERS; i++){
        reindeers_id[i] = i;
        pthread_create(&reindeers_threads[i], NULL, &reindeer, &reindeers_id[i]);
    }

    // Elves
    int* elves_id = calloc(ELVES, sizeof(int));
    pthread_t* elves_threads = calloc(ELVES, sizeof(pthread_t));
    for (int i = 0; i < ELVES; i++){
        elves_id[i] = i+1;
        pthread_create(&elves_threads[i], NULL, &elf, &elves_id[i]);
    }


	// Join santa
    pthread_join(santa_thread, NULL);
    return 0;
}