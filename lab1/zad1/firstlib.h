
#ifndef firstlib_h
#define firstlib_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


typedef struct{
	
    char **lines;
    int counter;
    
} MergedBlock;


typedef struct {
	
    MergedBlock **arrayOfBlocks;
    int size;
    
} MainArrayOfBlocks;



MainArrayOfBlocks* createMainArray(int size);

char** mergeFiles(int size, char* sequence);

int createBlock(MainArrayOfBlocks* mainArray, char* tmpFile);

int getNumberOfLines(MainArrayOfBlocks* mainArray, int index);

void removeBlock(MainArrayOfBlocks* mainAray, int index);

void removeRow(MainArrayOfBlocks* mainArray, int index, int row);

void printBlocks(MainArrayOfBlocks* mainArray);

int findFreeIndex(MainArrayOfBlocks* mainArray);

void cleanAll(MainArrayOfBlocks* mainArray);

#endif //firstlib_h
