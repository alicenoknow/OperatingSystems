
#ifndef config_h
#define config_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/shm.h> 
#include <sys/ipc.h>
#include <sys/sem.h>

// PIZZERIA PARAMETERS
#define COOKS_NO 1
#define DELIVERERS_NO 1
#define OVEN_CAPACITY 3
#define TABLE_CAPACITY 3
#define PIZZA_TYPES 9

#define PROJ_PATH getenv("HOME")
#define PROJ_ID 420

#define OVEN_SEM 0
#define TABLE_SEM 1


// Pizzeria
typedef struct Pizzeria{
	
    int oven[OVEN_CAPACITY];
    int table[TABLE_CAPACITY];
    int oven_occupied;
    int table_occupied;
    
} Pizzeria;

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
};


#endif //config_h
