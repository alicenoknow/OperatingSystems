#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h> 


int read_line(int fd, int chunk, char* line) {
    
    char ch;
    size_t read_bytes = chunk + 5;
	int i = 0;

    while(i < read_bytes){
        while(i < read_bytes && read(fd, &ch, 1) != 0){
			if(ch == '\n')
				break;
            line[i++] = ch;      
        }
		
		if(i == read_bytes){
			line = realloc(line, read_bytes*2);
			read_bytes *= 2;
		}
		else break;
    }
    return i;
}


void rewrite(char* file_path, int row, char* content, int chunk){
	int fd = open(file_path, O_RDONLY);
	FILE* tmp = fopen("tmp.txt", "w+");
	int count = 1;
	char* line = calloc(chunk+5, sizeof(char));
	char* newline;

	while(read_line(fd, chunk, line) != 0){
		if(count != row){
			fputs(line, tmp);
			fputc('\n', tmp);
		}
		else{
			newline = calloc(strlen(line) + strlen(content), sizeof(char));
			sprintf(newline, "%s%s\n", line, content);
			fputs(newline, tmp);
		}
		count++;
	}

    while (count < row){
        fputc('\n', tmp);
        count++;
    }

    if(count == row){
        fputs(content, tmp);
        fputc('\n', tmp);
        count++;
    }

	close(fd);
	fclose(tmp);
	remove(file_path);
    rename("tmp.txt", file_path);
}

int main(int argc, char** argv){

	if(argc != 4){
		printf("Wrong number of arguments!\n");
		exit(1);
	}
	
	char* pipe_path = argv[1];
	char* file_path = argv[2];
	int buf = atoi(argv[3]);
	
	int pipe_fd = open(pipe_path, O_RDONLY);
	char* line = calloc(buf + 5, sizeof(char));
	char* token, *output;
	int row;
	
	FILE* created = fopen(file_path, "w+");
	fclose(created);
	
	while(read_line(pipe_fd, buf, line) != 0){
		token = strtok(line, ":");
		row = atoi(token);
		output = strtok(NULL, ":");
		rewrite(file_path, row, output, buf);
	}
	
	free(line);
	close(pipe_fd);
	return 0;
}

