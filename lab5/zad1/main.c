#include "lib.h"



int main(int argc, char** argv){
	
	if(argc < 2){
		printf("Wrong number of arguments!\n");
		exit(0);
	}
	
	char* filename = argv[1];
	
	perform(filename);
	
	return 0;
}
