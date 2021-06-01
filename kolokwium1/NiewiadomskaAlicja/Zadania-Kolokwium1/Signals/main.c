#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


// My code

void sighandler(int sig, siginfo_t *info, void *ucontext){
	
	printf("Child got signal %d with value %d\n", sig, info->si_value.sival_int);
}

// End of my code

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;
    
    // My code
    
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);    
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);

	
    sigset_t blocked;
    sigemptyset(&blocked);
    sigfillset(&blocked);
    sigdelset(&blocked, SIGUSR1);
    sigdelset(&blocked, SIGUSR2);
	sigprocmask(SIG_BLOCK,&blocked, NULL);
	
	// End of my code
	
    //..........
	//zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1 i SIGUSR2
    //zdefiniuj obsluge SIGUSR1 i SIGUSR2 w taki sposob zeby proces potomny wydrukowal
    //na konsole przekazana przez rodzica wraz z sygnalami SIGUSR1 i SIGUSR2 wartosci
    

    int child = fork();
    if(child == 0) {
        sleep(1);
    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
        
        // My code
        
        int sig_num = atoi(argv[2]);
        union sigval val;
        val.sival_int = atoi(argv[1]);
        sigqueue(child, sig_num, val);
        
        // End of my code
    }

    return 0;
}
