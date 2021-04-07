#include "libka.h"


int first_signal(char* mode){
	if(!strcmp(mode, "sigrt"))
		return SIGRTMIN + 1;
	return SIGUSR1;
}

int second_signal(char* mode){
	if(!strcmp(mode, "sigrt"))
		return SIGRTMIN + 2;
	return SIGUSR2;
}

sigset_t set_mask(char* mode){
	sigset_t mask;	
	sigfillset(&mask);
	sigdelset(&mask, first_signal(mode));
	sigdelset(&mask, second_signal(mode));
	sigprocmask(SIG_SETMASK, &mask, NULL);
	
	return mask;
}


