#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h> 


int main(int argc, char** argv){

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
	char* out = calloc(buf+5, sizeof(char));
	
	while(fread(buffer, 1, sizeof(buffer), resources) > 0){
		sleep((rand()%2)+1);
		sprintf(out, "%d:%s\n", row, buffer);
        write(pipe_fd, out, strlen(out));
	}
	
	free(buffer);
	close(pipe_fd);
	fclose(resources);
	return 0;
}
