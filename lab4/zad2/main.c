#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void sigint_handler(int sig, siginfo_t *info, void *ucontext){
	printf("\nSIGINT handler\n");
	printf("Signal number: %d\n", info->si_signo);
	printf("PID of sending process: %d\n", info->si_pid);
    printf("Signal code: %d\n", info->si_code);
    kill(info->si_pid, SIGINT);
}

void sigchld_handler(int sig, siginfo_t *info, void *ucontext){
	printf("\nSIGCHLD handler\n");
	printf("Signal number: %d\n", info->si_signo);
	printf("PID of sending process: %d\n", info->si_pid);
    printf("Signal code: %d\n", info->si_code);
    exit(0);
}


void sigint_handler_child(int sig, siginfo_t *info, void *ucontext){
	printf("\nSIGINT handler child\n");
	printf("Signal number: %d\n", info->si_signo);
	printf("PID of sending process: %d\n", info->si_pid);
    printf("Signal code: %d\n", info->si_code);
    exit(0);
}

void sigchld_handler2(int sig, siginfo_t *info, void *ucontext){
	printf("\nParent process received SIGCHLD signal\n");
    exit(0);
}

void sigint_handler2(int sig, siginfo_t *info, void *ucontext){
	printf("\nSIGINT signal received\n");
}

void SIGINFO_test(){
    printf("\n\nSA_SIGINFO\n");

    struct sigaction sig_act;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_sigaction = sigint_handler;
    sig_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sig_act, NULL);

    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_sigaction = sigchld_handler;
    sig_act.sa_flags = SA_SIGINFO;
    sigaction(SIGCHLD, &sig_act, NULL);

    if(fork() == 0){
        signal(SIGINT, (__sighandler_t) &sigint_handler_child);
        kill(getppid(), SIGINT);
        pause();
    }
	else while(1){};
}


// after setting SA_NOCLDSTOP flag, parent process will get SIGCHLD only whem child process has terminated, not when stopped
void NOCLDSTOP_test(){
	printf("\n\nNOCLDSTOP\n");

    struct sigaction sig_act;	
	sigemptyset(&sig_act.sa_mask);
    sig_act.sa_sigaction = sigchld_handler2;
    sig_act.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sig_act, NULL);
    
    pid_t pid = fork();
    if(pid == 0){ while(1){}; }
	else{
		printf("Sending SIGSTOP to child process\n");
		kill(pid, SIGSTOP);
		printf("Sending SIGINT to child process\n");
		kill(pid, SIGINT);
		while(1){};
	}
	return;
}


// Resets signal handler to default after entering custom signal handler first time
void RESETHAND_test(){
	printf("\n\nRESETHAND\n");

    struct sigaction sig_act;	
	sigemptyset(&sig_act.sa_mask);
    sig_act.sa_sigaction = sigint_handler2;
    sig_act.sa_flags = SA_RESETHAND;
    sigaction(SIGINT, &sig_act, NULL);
    
    printf("Raising SIGINT\n");
    raise(SIGINT);
    printf("Raising SIGINT\n");
    raise(SIGINT);
	return;
}


int main(int argc, char** argv){
	
	if(argc < 2){
		printf("Not enough argumetns!\n");
		exit(0);
	}
	
	char* test = argv[1];
	
	if(!strcmp(test, "SIGINFO"))
		SIGINFO_test();
	else if(!strcmp(test, "NOCLDSTOP"))
		NOCLDSTOP_test();
	else if(!strcmp(test, "RESETHAND"))
		RESETHAND_test();
    else printf("Argument not valid!\n");

    return 0;
}
