#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

char* n = "10";

void run_consumer(char* file_path){

    pid_t pid;
    
    char** consumer = (char**) calloc(5, sizeof(char*));
    consumer[0] = "./consumer";
    consumer[1] = "./fifo";
    consumer[2] = file_path;
    consumer[3] = n;
    consumer[4] = NULL;
    
    if((pid = fork()) == 0){
		if(execvp(consumer[0], consumer) == -1)
			fprintf(stderr, "Error while using execlp() in main: %s\n", strerror(errno));
		
		for(int i=0; i < 5; i++)
			free(consumer[i]);
		free(consumer);
		exit(0);
	}	
}


void run_producer(char* file_path, char* row){
	
    pid_t pid;
    
    char** producer = (char**) calloc(6, sizeof(char*));
    producer[0] = "./producer";
    producer[1] = "./fifo";
    producer[2] = row;
    producer[3] = file_path;
    producer[4] = n;
    producer[5] = NULL;
    
    if((pid = fork()) == 0){
		if(execvp(producer[0], producer) == -1)
			fprintf(stderr, "Error while using execlp() in main: %s\n", strerror(errno));
		
		for(int i=0; i < 6; i++)
			free(producer[i]);
		free(producer);
	}	
}


int main(int argc, char** argv){

	int children = 3;
	
	mkfifo("fifo", S_IRUSR | S_IWUSR);
	printf("Fifo created\n");
	run_consumer("result.txt");
	printf("Consumer created\n");
    run_producer("./data/r1.txt", "1");
    printf("P1 created\n");
    run_producer("./data/r2.txt", "2");
    printf("P2 created\n");
    //~ run_producer("./data/r3.txt", "3");
    //~ printf("P3 created\n");
    //~ run_producer("./data/r4.txt", "4");
    //~ printf("P4 created\n");
	
	for (int i = 0; i < children; i++) {wait(NULL);}
	printf("Main errnor %s\n", strerror(errno));
	int returnValue = unlink("fifo");
        if(returnValue==0)
            printf("FIFO deleted.\n");
	
	return 0;
}
