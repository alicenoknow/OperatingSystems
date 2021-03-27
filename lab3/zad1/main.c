#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char** argv){

	if(argc < 2){
		perror("Not enough arguments!\n");
		exit(0);
	}
	
	int n = atoi(argv[1]);
	int pid = -1, status;
	
	for(int i=0; i<n; i++){
        if((pid = fork()) == 0){
            printf("[I am child with pid: %d\n", getpid());
            exit(0);
        }
        else if(pid == -1){
			perror("Error while forking!\n");
			exit(0);
		}
    }
    
	waitpid(pid, &status, 0);
	return 0;
}
