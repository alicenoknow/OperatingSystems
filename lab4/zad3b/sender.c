#include "libka.h"

int signals_to_send, received_counter, received_sigusr2, sent_counter;
pid_t catcher;
char* mode;


void send_signal(int signal){
	
	if(!strcmp(mode, "sigqueue")){
		
		union sigval un_sig;

		sigqueue(catcher, signal, un_sig);
	}
	else
		kill(catcher, signal);

}

void sigusr1_handler(int sig_no,  siginfo_t* info, void* ucontext){
	
	
	if(sent_counter == signals_to_send)
		send_signal(second_signal(mode));
	else{
		sent_counter++;
		send_signal(first_signal(mode));
	}
	
	received_counter++;
	
	if(!strcmp(mode, "sigqueue")){
        printf("Catcher confirms receiving signal %d\n", info -> si_value.sival_int);
    }
}


void sigusr2_handler(int sig_no,  siginfo_t* info, void* ucontext) {

    printf("Sender sent %d signals.\nSender received back %d signals from catcher.\n", signals_to_send, received_counter);
    received_sigusr2 = 1;
    
    exit(0);
}


int main(int argc, char** argv){

	if(argc < 4){
		perror("Not enough arguments!\n");
		exit(0);
	}
	
	signals_to_send = atoi(argv[1]);
	catcher = atoi(argv[2]);
	mode = argv[3];
	
	if(strcmp(mode, "kill") && strcmp(mode, "sigqueue") && strcmp(mode, "sigrt")){
		printf("Thrid argument should be kill, sigqueue or sigrt!\n");
		exit(0);
	}
	
	sigset_t mask = set_mask(mode);
	
	printf("Sender PID: %d\n", getpid());
		
	struct sigaction act_sigusr1;
    act_sigusr1.sa_flags = SA_SIGINFO;
    act_sigusr1.sa_sigaction = &sigusr1_handler;
    sigaction(first_signal(mode), &act_sigusr1, NULL);

    struct sigaction act_sigusr2;
    act_sigusr2.sa_flags = SA_SIGINFO;
    act_sigusr2.sa_sigaction = &sigusr2_handler;
    sigaction(second_signal(mode), &act_sigusr2, NULL);
    
	send_signal(first_signal(mode));
	
	while(!received_sigusr2)
		sigsuspend(&mask);

	
	return 0;
}
