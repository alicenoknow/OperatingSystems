#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

int n = 10;

void run_consumer(char* file_path){

    pid_t pid;
    
    if((pid = fork()) == 0){
		if (execlp("./consumer", "./consumer", "fifo", file_path, n, NULL) == -1)
			fprintf(stderr, "Error while using execlp() in main: %s\n", strerror(errno));
		exit(0);
	}
}

./producer fifo 1 ./data/r1.txt 10

void run_producer(char* file_path, int row){
	pid_t pid;
	char** args = calloc(6, sizeof(char*));
	args[0] = calloc(15, sizeof(char))
	if((pid = fork()) == 0){
		if (execlp("./producer", "./producer", "fifo", row, file_path, n, NULL) == -1)
		fprintf(stderr, "Error while using execlp() in main: %s\n", strerror(errno));
		exit(0);
	}
}


int main(int argc, char** argv){

	int children = 5;
	
	mkfifo("fifo", S_IRUSR | S_IWUSR);
	
	run_consumer("result.txt");
    run_producer("./data/r1.txt", 1);
    run_producer("./data/r2.txt", 2);
    run_producer("./data/r3.txt", 3);
    run_producer("./data/r4.txt", 4);
	
	for (int i = 0; i < children; i++) {wait(NULL);}
	
	return 0;
}
