#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>


char* child_name = "child";
int sig_no = 9;   // 10 - SIGUSR1, 2 - SIGINT, 9 - SIGKILL


static void sig_handler(int sig_no){
	printf(" Signal received and handled. \n");
}


void exec_child(char* opt){
	
	char** args = calloc(3, sizeof(char*));
	args[0] = calloc(10, sizeof(char));
	args[1] = calloc(5, sizeof(char));
	args[2] = NULL;
	
	sprintf(args[0], "%s", opt);
	sprintf(args[1], "%d", sig_no);
	
	if(execv("./child", args) == -1){
		free(args[0]);
		free(args[1]);
		free(args);
		
		printf("Error while creating child process!\n");
		exit(1);
	}
	
	free(args[0]);
	free(args[1]);
	free(args);
}


void check_pending(char* name){
	
	sigset_t check;
	sigpending(&check);
	if (sigismember(&check, sig_no))
		printf("Signal pending in %s\n", name);
	else
		printf("Signal not pending in %s\n", name);
}


void ignore_test(int fork_child){
	signal(sig_no, SIG_IGN);
	
	printf("Raising signal in parent\n");
	raise(sig_no);
	
	if(fork_child){
		if(fork() == 0){
            
		printf("Raising signal in child\n");
		raise(sig_no);
		}
		wait(NULL);
	}
	else{
		exec_child("ignore");
	}
}


void handler_test(){
	signal(sig_no, sig_handler);
	
	printf("Raising signal in parent\n");
	raise(sig_no);

	if(fork() == 0){
			
		printf("Raising signal in child\n");
		raise(sig_no);
		return;
	}
	
	wait(NULL);
}


void pending_test(int fork_child){
	
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, sig_no);
	if(sigprocmask(SIG_SETMASK, &mask, NULL) == -1){
		printf("Error while masking\n");
		exit(1);
	}
	
	printf("Raising signal in parent\n");
	raise(sig_no);
	
	check_pending("parent");
	
	
	if(fork_child){
		if(fork() == 0){
			
			check_pending("child");
			return;
		}
	}
	else
		exec_child("pending");
		
	wait(NULL);
	
}


void mask_test(int fork_child){
	sigset_t mask;
	
	sigemptyset(&mask);
	sigaddset(&mask, sig_no);
	if(sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
		printf("Error while masking\n");
		exit(1);
	}
	
	printf("Raising signal in parent\n");
	raise(sig_no);
	
	check_pending("parent");
	
	if(fork_child){
		if(fork() == 0){
				
			printf("Raising signal in child\n");
			raise(sig_no);
			
			check_pending("child");		
			
			return;
		}
	}
	else
		exec_child("mask");

	wait(NULL);
}



int main(int argc, char** argv){
	
	if(argc < 2){
		printf("Wrong number of arguments!\n");
		exit(0);
	}
	
	if(!strcmp(argv[1], "ignore")){
		printf("\nFORK IGNORE TEST\n");
		ignore_test(1);
		printf("\nEXEC IGNORE TEST\n");
		ignore_test(0);

	}	
	else if(!strcmp(argv[1], "handler")){
		printf("\nFORK HANDLER TEST\n");
		handler_test();
	}
	else if(!strcmp(argv[1], "mask")){
		printf("\nFORK MASK TEST\n");
		mask_test(1);
		printf("\nEXEC MASK TEST\n");
		mask_test(0);
	}
	else if(!strcmp(argv[1], "pending")){
		printf("\nFORK PENDING TEST\n");
		pending_test(1);
		printf("\nEXEC PENDING TEST\n");
		pending_test(0);
	}
	else{
		printf("Wrong argument! Allowed options: ignore, handler, mask, pending\n");
		exit(0);
	}
	
	return 0;
}
