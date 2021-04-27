#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/file.h>
#include <errno.h>
#include <unistd.h> 
#include <fcntl.h> 


int main(int argc, char** argv){

	printf("Producer runnng\n");
	if(argc != 5){
		printf("Wrong number of arguments!\n");
		exit(1);
	}
	
	char* pipe_path = argv[1];
	int row = atoi(argv[2]);
	char* file_path = argv[3];
	int buf = atoi(argv[4]);
	
	FILE* resources = fopen(file_path, "r");
	int pipe_fd = open(pipe_path, O_WRONLY);
	char* buffer = calloc(buf, sizeof(char));
	char* output = calloc(buf+5, sizeof(char));
	
	printf("Producer runnng\n");
	
	while(fgets(buffer, sizeof(buffer), resources) > 0){
		
		sleep((rand()%2)+1);
		
		sprintf(output, "%d:%s\n", row, buffer);
		flock(pipe_fd, LOCK_EX);
        write(pipe_fd, output, strlen(output));
        flock(pipe_fd, LOCK_UN);
        
        printf("Producer %d writing %s\n", row, output);

	}
	
	free(buffer);
	close(pipe_fd);
	fclose(resources);
	return 0;
}
