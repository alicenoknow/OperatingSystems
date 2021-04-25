#include "lib.h"


void remove_spaces(char *s){

	char * p = s;
    int l = strlen(p);
    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;
    memmove(s, p, l + 1);

	return;
}


// takes string "cmd arg1 arg2 ..." and parse it into Command structure
Command* parse_command(char* start){
	
	Command* command = calloc(1, sizeof(Command));
	command->arg_num = 0;
	command->args = calloc(MAX_ARGS+2, sizeof(char*));

	
    char* token = strtok(start, " ");
    remove_spaces(token);   
    command->args[0] = calloc(strlen(token) + 1, sizeof(char));
    strcpy(command->args[0], token);
    command->arg_num++;

    while((token = strtok(NULL, " ")) != NULL) {
		command->args[command->arg_num] = calloc(strlen(token)+1, sizeof(char));
		remove_spaces(token);
        strcpy(command->args[command->arg_num], token);
        command->arg_num++;
    }
    command->args[command->arg_num] = NULL;
	
    return command;
}


// takes line "name = cmd1 arg1.1 arg1.2 ... | cmd2 arg2.1 arg2.2 ..." and
// parse it into Component structure
Component* parse_component(char* start){
	Component* component = calloc(1, sizeof(Component));
	component->commands = calloc(1, sizeof(Command));
	component->commands_num = 0;
	component->name = calloc(MAX_COMPONENT_NAME, sizeof(char));
	
	char** tmp = calloc(MAX_COMMANDS_SEQ, sizeof(char*));
    char* token = strtok(start, "=");
    
    remove_spaces(token);
	strcpy(component->name, token);

     while((token = strtok(NULL, "|")) != NULL){
		
		tmp[component->commands_num] = calloc(strlen(token), sizeof(char));
	    remove_spaces(token);
		strcpy(tmp[component->commands_num], token);
		component->commands_num++;
    }
	
	component->commands = calloc(component->commands_num, sizeof(Command*));
	for(int i =0; i < component->commands_num; i++)
		component->commands[i] = parse_command(tmp[i]);
	
	free(tmp);
    return component;
	
}

Component* find_component(Component** components, int components_num, char* name){
	
	for(int i = 0; i < components_num; i++)
		if(!strcmp(name, components[i]->name))
			return components[i];
	return NULL;
}

// takes line "cmp1 | cmp2 ..." and
// parse it into Sequence structure
Sequence* parse_sequence(char* start, Component** components, int components_num){
	
	Sequence* sequence = calloc(1, sizeof(Sequence));
	sequence->components = calloc(MAX_COMMANDS_SEQ, sizeof(Component));
	sequence->components_num = 0;
	sequence->total_commands_num = 0;
	
	Component* next_component;
    char* token = strtok(start, "|");
    
    remove_spaces(token);
	next_component = find_component(components, components_num, token);
	if(next_component){
		sequence->components[sequence->components_num] = next_component;
		sequence->components_num++;
		sequence->total_commands_num += next_component->commands_num;
	}

     while((token = strtok(NULL, "|")) != NULL){
	    remove_spaces(token);
		next_component = find_component(components, components_num, token);
		
		if(next_component){
			sequence->components[sequence->components_num] = next_component;
			sequence->components_num++;
			sequence->total_commands_num += next_component->commands_num;
		}
    }
    return sequence;
}


void free_command(Command* command){

	for(int i = 0; i < MAX_ARGS+1; i++)
		free(command->args[i]);
	free(command->args);
	free(command);
}

void free_component(Component* component){

	for(int i = 0; i < component->commands_num; i++)
		free_command(component->commands[i]);
	free(component->commands);
	free(component->name);
	free(component);
}

void free_sequence(Sequence* sequence){
	free(sequence->components);
	free(sequence);
}


void perform_sequence(Sequence* sequence){
	
	int cmd_counter = 1;
	int pipe_curr[2];
	int prev = STDIN_FILENO;
	pid_t pid;
	
	
	for(int i = 0; i < sequence->components_num; i++)
		for(int j = 0; j < sequence->components[i]->commands_num; j++){
			Command* cmd = sequence->components[i]->commands[j];
			
			pipe(pipe_curr);
			
			if ((pid = fork()) == 0) {
				
				dup2(prev, STDIN_FILENO);
				if(prev != STDIN_FILENO)
					close(prev);
				
				if(cmd_counter < sequence->total_commands_num){
					dup2(pipe_curr[WRITE], STDOUT_FILENO);
					close(pipe_curr[WRITE]);
				}
				execvp(cmd->args[0], cmd->args);
			}
			
			if (cmd_counter < sequence->total_commands_num) {
				close(prev);
				close(pipe_curr[WRITE]);
				prev = pipe_curr[READ];
			}
			
			cmd_counter++;
		}
	 while (wait(NULL) > 0);
}

void run_sequences(Sequence** sequences, int seq_num){

	for(int i = 0; i < seq_num; i++)
		perform_sequence(sequences[i]);
}

// parse whole file into ready to use sequences of components
void perform(char* filename){
	
	int components_num = 0;
	Component** components = calloc(1, sizeof(Component*));
	Component* new_component;
	
	int sequences_num = 0;
	Sequence** sequences = calloc(1, sizeof(Sequence*));
	Sequence* new_sequence;
	
	FILE* fd;
	if((fd = fopen(filename, "r")) == NULL){
		perror("Error while opening file with commands!\n");
		exit(1);
	}
	
	char* line;
    size_t len = 0;
    while((getline(&line, &len, fd)) != -1){
		if ('\n' == line[0])
			continue;
		
		char *command_start = strchr(line, '=');
		line[strcspn(line, "\n")] = 0;
		
		if(command_start){
			
			new_component = parse_component(line);
			components_num++;
			components = realloc(components, components_num * sizeof (Component*));
			components[components_num-1] = new_component;
		}
		else{
			new_sequence = parse_sequence(line, components, components_num);
			sequences_num++;
			sequences = realloc(sequences, sequences_num * sizeof (Sequence*));
			sequences[sequences_num-1] = new_sequence;
		}
	}
	
	run_sequences(sequences, sequences_num);
	

	for(int i = 0; i < sequences_num; i++)
		free_sequence(sequences[i]);
	for(int i = 0; i < components_num; i++)
		free_component(components[i]);
		
	fclose(fd);
}

