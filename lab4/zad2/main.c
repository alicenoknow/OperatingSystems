#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


static void info_handler(int sig_no, siginfo_t* info, void* ucontext){
	
	printf("Signal number: %d\n", info->si_signo);
	printf("PID of sending process: %d\n", info->si_pid);
    printf("Signal code: %d\n", info->si_code);
    printf("Real user ID of sending process: %d\n", info->si_uid);
    printf("Signal value: %d\n", info->si_value.sival_int);
}


void sig_info(){
	struct sigaction sig_act;
	
    sig_act.sa_sigaction = &info_handler;
    sig_act.sa_flags = SA_SIGINFO;
    
    sigaction(SIGUSR1, &sig_act, NULL);
    
    raise(SIGUSR1);
}


int main(int argc, char* argv[]){
	
	printf("SA_SIGINFO\n");
	sig_info();
	raise(SIGUSR1);
	
	return 0;
}
