#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>


// Checks if file s .txt file
int is_txt(const char *path);

// Browse directory for given phrase, no deeper than depth
void browse_dir(char *path, char *from_root, char* phrase, int depth);

//Browse txt file for given phrase
void browse_txt(char* path, char *from_root, char* phrase);


int main(int argc, char** argv){

	if(argc < 4){
		perror("Not enough arguments!\n");
		exit(0);
	}
	
	char* root = argv[1];
	char* phrase = argv[2];
	int depth = atoi(argv[3]);
	char* path = calloc(300, sizeof(char));
	char* relative_path = calloc(300, sizeof(char));	
	strcpy(path, realpath(root, NULL));
	
    browse_dir(path, relative_path, phrase, depth);
	
	free(path);
	free(relative_path);
	
	return 0;
}


void browse_dir(char *path, char *from_root, char* phrase, int depth){
		
	if(depth == -1)
		exit(0);
	
    struct dirent *dp;
    DIR *dir = opendir(path);
    int pid = -1, status;
    


    if(!dir){
		perror("Given path is not valid! \n");
		exit(0);
	} 
	
	
    while((dp = readdir(dir)) != NULL){
		if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
			continue;
		
		char* relative = calloc(300, sizeof(char));
		char* new_path = calloc(300, sizeof(char));
		
		sprintf(relative, "%s/%s", from_root, dp->d_name);
		sprintf(new_path, "%s/%s", path, dp->d_name);
		
        if(dp->d_type == DT_DIR){
			
			if((pid = fork()) == 0){
				browse_dir(new_path, relative, phrase, depth-1);
				exit(0);
			}
			continue;
		}
		
        if(is_txt(dp->d_name))
			browse_txt(new_path, relative, phrase);
		
		free(relative);
		free(new_path);
    }
	waitpid(pid, &status, 0);
    closedir(dir);
    
}


int is_txt(const char *path){
	size_t len = strlen(path);
	
    if(len > 4 &&
        path[len - 4] == '.' &&
        path[len - 3] == 't' &&
        path[len - 2] == 'x' &&
        path[len - 1] == 't')
        
		return 1;
	else return 0;
}

void browse_txt(char* path, char *from_root, char* phrase){
	FILE* fd = fopen(path, "r");
	size_t len;
	char* line = calloc(200, sizeof(char));
	int counter = 0;
	
	while((getline(&line, &len, fd)) != -1)
        if(strstr(line, phrase) != NULL)
			counter++;
	
	if(counter)
		printf("\nProccess with PID: %d found phrase: \"%s\" occuring %d times in : %s (relative path from given directory)\n", getpid(), phrase, counter, from_root);
	
	fclose(fd);
	free(line);
}
