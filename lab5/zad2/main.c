#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv){
	
	FILE* pipe;
	char* command = calloc(500, sizeof(char));
	
	if(argc == 2){
		
		if(!strcmp("nadawca", argv[1])){
			// usun pierwsza i ostatnia linie, posortuj po 3 kolumnie
			command = calloc(50, sizeof(char));
			sprintf(command, "echo | mail | sed '1d;$d' | sort -k3");
			
			if ((pipe = popen(command, "r")) == NULL) {
				printf("Error while opening pipe to read sorted by sender mails!\n");
				exit(1);
			}			
		}
		else if(!strcmp("data", argv[1])){
			command = calloc(50, sizeof(char));
			sprintf(command, "echo | mail | sed '1d;$d' | sort -k5M -k6 -k7");
			
			if ((pipe = popen(command, "r")) == NULL) {
				printf("Error pening pipe to read sorted by date mails!\n");
				exit(2);
			}		
		}
		else{
			printf("Wrong argument given!\n");
			exit(3);
		}
		
		char* line = calloc(500, sizeof(char));
        while(fgets(line, 500, pipe) != NULL)
			printf("%s", line);
        free(line);
	}
	else if(argc == 4){
		char* mail = argv[1];
		char* title = argv[2];
		char* body = argv[3];
		
		command = calloc(10 + strlen(title) + strlen(mail) + strlen(body), sizeof(char));
        sprintf(command, "echo '%s' | mail -s '%s' %s", title, body, mail);
        pipe = popen(command, "r");
        
        if(pipe == NULL){
			printf("Error while opening pipe to send mail!\n");
			exit(4);
		}       
	}
	
	else{
		printf("Wrong number of arguments!\n");
		exit(2);
	}
	
	pclose(pipe);
    free(command);
	
    return 0;
}
