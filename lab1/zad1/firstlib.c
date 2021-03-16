#include "firstlib.h"


MainArrayOfBlocks* createMainArray(int size){
	
	MainArrayOfBlocks* arr = calloc(1, sizeof(MainArrayOfBlocks));
	arr->arrayOfBlocks = calloc(size, sizeof(MergedBlock*));
	arr->size = size;
	return arr;
}



char** mergeFiles(int size, char* sequence){
	
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


int createBlock(MainArrayOfBlocks* mainArray, char* tmpFile){

	FILE* f;
	size_t len = 0;
	int counter = 0;
	
	int index = findFreeIndex(mainArray);
	if(index == -1){
		perror("Main array is full! Cannot create new block!");
		exit(1);
	}
	
	char* line = calloc(200, sizeof(char));
	
	f = fopen(tmpFile, "r");	
	while(getline(&line, &len, f) != -1){
		counter++;
	}
	fclose(f);

	MergedBlock* newBlock = calloc(1, sizeof(MergedBlock));
	newBlock->lines = (char**)calloc(counter, sizeof(char*));
	newBlock->counter = counter;
	
	f = fopen(tmpFile, "r");
	for(int i = 0; i < counter; i++)	{
		if(getline(&(newBlock->lines[i]), &len, f) == -1){
			perror("Error while creating new block! \n");
			exit(1);
		}
	}
	
	fclose(f);
	free(line);
	mainArray->arrayOfBlocks[index] = newBlock;
	
	return index;
}

int getNumberOfLines(MainArrayOfBlocks* mainArray, int index){
	if(mainArray->arrayOfBlocks[index] == NULL)
		return -1;
		
	return mainArray->arrayOfBlocks[index]->counter;
}


void removeBlock(MainArrayOfBlocks* mainArray, int index){
	if(mainArray->arrayOfBlocks[index] == NULL)
		return;
	
	int counter = mainArray->arrayOfBlocks[index]->counter;
	
	for(int i = 0; i < counter; i++){
		free(mainArray->arrayOfBlocks[index]->lines[i]);
	}

	free(mainArray->arrayOfBlocks[index]->lines);
	mainArray->arrayOfBlocks[index] = NULL;
}


void removeRow(MainArrayOfBlocks* mainArray, int index, int row){
	int counter = getNumberOfLines(mainArray, index);
	
	if(counter < 1 || row >= counter)
		return;
		
	free(mainArray->arrayOfBlocks[index]->lines[row]);
	mainArray->arrayOfBlocks[index]->lines[row] = NULL;
}


void printBlocks(MainArrayOfBlocks* mainArray){
	int size = mainArray->size;
	int lines;
	
	for(int i = 0; i < size; i++){
		
		lines = getNumberOfLines(mainArray, i);
		if(lines == -1)
			continue;
			
		printf("Blok ID %d:\n", i);
		for(int j = 0; j < lines; j++){
			if(mainArray->arrayOfBlocks[i]->lines[j] == NULL)
				continue;
			printf("\t %s \n", mainArray->arrayOfBlocks[i]->lines[j]);
		}
	}

}

void cleanAll(MainArrayOfBlocks* mainArray){
	int size = mainArray->size;
	
	for(int i = 0; i < size; i++)
		removeBlock(mainArray, i);
				
	free(mainArray->arrayOfBlocks);
	free(mainArray);
}

int findFreeIndex(MainArrayOfBlocks* arr){
	for(int i = 0; i < arr->size; i++)
		if(arr->arrayOfBlocks[i] == NULL)
			return i;
	return -1;
}

