
#ifndef libka_h
#define libka_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int first_signal(char* mode);
int second_signal(char* mode);

sigset_t set_mask(char* mode);


#endif //libka_h
