#include "firstlib.h"
#include <time.h>
#include <sys/times.h>


int main(int argc, char* argv[]){
	MainArrayOfBlocks* arr;
	int size = 0;
	char* sequance;
	char** tmp;
	
	for(int i = 1; i < argc; i++){
		
		if(strcmp(argv[i], "createArray") == 0){
			size = atoi(argv[++i]);
			arr = createMainArray(size);
			
			continue;
		}
		
		if(strcmp(argv[i], "mergeFiles") == 0){
			i++;
			sequance = calloc(size*10, sizeof(char));
			tmp = calloc(size, sizeof(char*));
			
			for(int j = i; j < i + size; j++){
				strcat(sequance, argv[j]);
				if(size > 1)
					strcat(sequance, " ");
			}
			
			i += size - 1;
			tmp = mergeFiles(size, sequance);
			
			continue;
		}
		
		if(strcmp(argv[i], "createBlock") == 0){
			int pair = atoi(argv[++i]);
			int id = createBlock(arr, tmp[pair]);
			printf("Created block %d\n", id);
			continue;
		}
		
		if(strcmp(argv[i], "removeBlock") == 0){
			int idx = atoi(argv[++i]);
			removeBlock(arr, idx);
			continue;
		}
		
		if(strcmp(argv[i], "removeRow") == 0){
			int idx = atoi(argv[++i]);
			int row = atoi(argv[++i]);
			
			removeRow(arr, idx, row);
			continue;
		}
		
		if(strcmp(argv[i], "printBlocks") == 0){
			printBlocks(arr);
			continue;
		}
		
		if(strcmp(argv[i], "addAndDelete") == 0){
			int cnt = atoi(argv[++i]);
			for(int j = 0; j < cnt; j++){
				int id = createBlock(arr, tmp[0]);
				removeBlock(arr, id);
			}
			continue;
		}
		
		perror("Invalid command! ");
		exit(1);
	}
	
	cleanAll(arr);
	
}


