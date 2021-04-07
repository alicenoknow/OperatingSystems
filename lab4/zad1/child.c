#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>


void check_pending(char* name, int signal){
	
	sigset_t check;
	sigpending(&check);
	if (sigismember(&check, signal))
		printf("Signal pending in %s\n", name);
	else
		printf("Signal not pending in %s\n", name);
}

void ignore_test(int signal){

	printf("Raising signal in child\n");
	raise(signal);
	return;

}


void pending_test(int signal){
	
	check_pending("child", signal);
	return;
	
}

void mask_test(int signal){
	     
	printf("Raising signal in child\n");
	raise(signal);
	
	check_pending("child", signal);		
	
	return;
}


int main(int argc, char** argv){
	
	if(argc < 2){
		printf("Wrong number of arguments!\n");
		exit(0);
	}
	
	int signal = atoi(argv[1]);
	
	if(!strcmp(argv[0], "ignore"))
		ignore_test(signal);
	
	else if(!strcmp(argv[0], "mask"))
		mask_test(signal);
	
	else if(!strcmp(argv[0], "pending"))
		pending_test(signal);
	
	else{
		printf("Wrong argument! Allowed options: ignore, mask, pending\n");
		exit(0);
	}
	
	
	
	return 0;
}

