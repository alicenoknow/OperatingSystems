
#ifndef lib_h
#define lib_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#define MAX_ARGS 3
#define MAX_COMMANDS_SEQ 3
#define MAX_COMPONENT_NAME 20
#define READ 0
#define WRITE 1


typedef struct Command{

	char** args;
	int arg_num;
	
} Command;


typedef struct Component{
	
	char* name;
	Command** commands;
	int commands_num;
	
} Component;

typedef struct Sequence{

	Component** components;
	int components_num;
	int total_commands_num;
	
} Sequence;

void perform(char* filenames);
void remove_spaces(char* s);

#endif //lib_h
