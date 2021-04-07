#include "libka.h"


int signals_to_send, received_counter, received_sigusr2;
char* mode;
pid_t sender;

void send_signals(){
	
	if(!strcmp(mode, "sigqueue")){
		
		union sigval un_sig;
		
		int i;
		for (i = 0; i < received_counter; i++){
			un_sig.sival_int = i+1;		

			sigqueue(sender, first_signal(mode), un_sig);
		}

		un_sig.sival_int = i+1;
		sigqueue(sender, second_signal(mode), un_sig);
	}
	else{
		for (int i = 0; i < received_counter; i++)
			kill(sender, first_signal(mode));
		
		kill(sender, second_signal(mode));
	}
}


void sigusr1_handler(int sig_no, siginfo_t* info, void* ucontext){
	received_counter++;
}

void sigusr2_handler(int sig_no, siginfo_t* info, void* ucontext) {
	
	sender = info -> si_pid;
	send_signals();
	printf("Catcher received and sent back %d signals\n", received_counter);

	
	received_sigusr2 = 1;
    exit(0);
}


int main(int argc, char** argv){

	if(argc < 2){
		perror("Not enough arguments!\n");
		exit(0);
	}
	
	mode = argv[1];
	
	sigset_t mask = set_mask(mode);
	
	struct sigaction act_sigusr1;
    act_sigusr1.sa_flags = SA_SIGINFO;
    act_sigusr1.sa_sigaction = &sigusr1_handler;
    sigaction(first_signal(mode), &act_sigusr1, NULL);

    struct sigaction act_sigusr2;
    act_sigusr2.sa_flags = SA_SIGINFO;
    act_sigusr2.sa_sigaction = &sigusr2_handler;
    sigaction(second_signal(mode), &act_sigusr2, NULL);
	
	printf("Catcher PID:%d\n", getpid());

	
	while(!received_sigusr2)
		sigsuspend(&mask);

	
	return 0;
}
