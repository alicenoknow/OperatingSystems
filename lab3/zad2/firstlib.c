#include "firstlib.h"

// mergeFiles1 - original from lab 1
// mergeFiles2 - fork added


// original mergeFiles function from lab 1, it gets all pairs to merge
// and merge them all sequentially in a loop
char** mergeFiles1(int size, char* sequence){
	
	if(size <= 0)
		return NULL;
	
	FILE *f1, *f2, *ftmp;
	size_t len1 = 0, len2 = 0;
	

	char* name1 = calloc(20, sizeof(char));
	char* name2 = calloc(20, sizeof(char));
	char* line1 = calloc(100, sizeof(char));
	char* line2 = calloc(100, sizeof(char));
	char** tmp_names = calloc(size, sizeof(char*));
	
	char *string = strdup(sequence);

	
	string = strtok(string, " :");

	for(int i  = 0; i < size && string != NULL; i++){
		
		strcpy(name1, string);
		string = strtok(NULL, " :");
		strcpy(name2, string);
		string = strtok(NULL, " :");
		
		tmp_names[i] = calloc(10, sizeof(char));
		sprintf(tmp_names[i],"tmp%d.txt",i);
		
		if( access( name1, F_OK ) == 0 && access( name2, F_OK ) == 0 ){
			f1 = fopen(name1, "r");
			f2 = fopen(name2, "r");
			ftmp = fopen(tmp_names[i], "w+");
		
			while (ftmp != NULL && f1 != NULL && f2 != NULL) {
				if(getline(&line1, &len1, f1) == -1){
					if(getline(&line2, &len2, f2) == -1)
						break;
					else{
						fputs(line2, ftmp);
					}
				}
				else if(getline(&line2, &len2, f2) == -1){
					fputs(line1, ftmp);
				}
				else{
					fputs(line1, ftmp);
					fputs(line2, ftmp);
				}
				
			}
			
			fclose(f1);
			fclose(f2);
			fclose(ftmp);
		}
		else{
			perror( "Cannot access given files!" );
		}
	}
		
		if(name1)
			free(name1);
		if(name2)
			free(name2);
		if(line1)
			free(line1);
		if(line2)
			free(line2);
		
		return tmp_names;

}


// Used with fork, merges only one pair of files at the time
char* mergeFiles2(char* files, int number){
	
	FILE *f1, *f2, *ftmp;
	size_t len1 = 0, len2 = 0;
	

	char* name1 = calloc(20, sizeof(char));
	char* name2 = calloc(20, sizeof(char));
	char* line1 = calloc(100, sizeof(char));
	char* line2 = calloc(100, sizeof(char));
	char* tmp_name = calloc(20, sizeof(char));
	
	char *string = strdup(files);
	string = strtok(string, " :");
		
	strcpy(name1, string);
	string = strtok(NULL, " :");
	strcpy(name2, string);
	string = strtok(NULL, " :");
	
	sprintf(tmp_name,"tmp%d.txt", number);
	
	if( access( name1, F_OK ) == 0 && access( name2, F_OK ) == 0 ){
		f1 = fopen(name1, "r");
		f2 = fopen(name2, "r");
		ftmp = fopen(tmp_name, "w+");
	
		while (ftmp != NULL && f1 != NULL && f2 != NULL) {
			if(getline(&line1, &len1, f1) == -1){
				if(getline(&line2, &len2, f2) == -1)
					break;
				else{
					fputs(line2, ftmp);
				}
			}
			else if(getline(&line2, &len2, f2) == -1){
				fputs(line1, ftmp);
			}
			else{
				fputs(line1, ftmp);
				fputs(line2, ftmp);
			}
			
		}
		
		fclose(f1);
		fclose(f2);
		fclose(ftmp);
	}
	else{
		perror( "Cannot access given files!" );
	}
	
	if(name1)
		free(name1);
	if(name2)
		free(name2);
	if(line1)
		free(line1);
	if(line2)
		free(line2);
	
	return tmp_name;

}

