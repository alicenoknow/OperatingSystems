#include "config.h"

// Headers 
void remove_queue();
void sigint_handler(int signum);
void signals_handling_setup();
void init();
void send_init();
void send_stop();
void send_connect(int interlocutor_id);
void send_list();
void send_disconnect();
void error_handle(char* message);
void handle_message(Message* received);
void handle_connect(Message* received);
void handle_disconnect(Message* received);
void handle_stop(Message* received);
void handle_list(Message* received);
void handle_chat(Message* received);
void send_message(char* message);
void chat();
void check_for_input();
int input_given();


// Global
key_t private_key = -1;
key_t server_key = -1;

int private_queue = -1;
int server_queue = -1;
int client_id = -1;
int interlocutor_queue = -1;


void remove_queue(){
	if(private_queue != -1)
		msgctl(private_queue, IPC_RMID, NULL);
}


void sigint_handler(int signum){
	send_stop();
}


void signals_handling_setup(){
	
	struct sigaction act;
    act.sa_flags = 0;
    sigfillset(&act.sa_mask);
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);
}


void init(){
	signals_handling_setup();
	
	char *path = getenv("HOME");
	if(path == NULL)
		error_handle("Failed to get enviromental variable HOME");
		
	private_key = ftok(path, getpid());
	if(private_key == -1)
		error_handle("Failed to create client private key");

	private_queue = msgget(private_key, 0666 | IPC_CREAT | IPC_EXCL);  // 0666 - permissions, IPC_CREAT - create if does not exist, IPC_EXCL - error if already exists
	if(private_queue == -1)
		error_handle("Failed to create client private queue");
	atexit(remove_queue);
	
	server_key = ftok(path, PROJ_ID);
	if(server_key == -1)
		error_handle("Failed to get server key");
		
	server_queue = msgget(server_key, 0);
	if(server_queue == -1)
		error_handle("Failed to open server queue");
	
	send_init();
}


void send_init(){
	printf("Client sending INIT\n");
	Message msg;
	msg.mtype = INIT;
	sprintf(msg.mtext, "%d", private_key);
	msgsnd(server_queue, &msg, MSG_SIZE, 0);
	
	Message new_message;
	
	if (msgrcv(private_queue, &new_message, MSG_SIZE, -10, 0) < 0)
        error_handle("Failed to read new message");
	
	if(new_message.mtype == INIT){
		if (sscanf(new_message.mtext, "%d", &client_id) < 0)
			error_handle("Failed to register client");
	}
	printf("Client ID %d\n", client_id);
		
}


void send_stop(){
	printf("Client sending STOP\n");
	if(server_queue != -1){
		Message msg;
		msg.mtype = STOP;
		msg.id = client_id;
		
		msgsnd(server_queue, &msg, MSG_SIZE, 0);
	}
	
	exit(0);
	
}


void send_connect(int interlocutor_id){
	printf("Client sending CONNECT\n");
	Message msg;
	msg.mtype = CONNECT;
	msg.id = client_id;
	sprintf(msg.mtext, "%d", interlocutor_id);
	msgsnd(server_queue, &msg, MSG_SIZE, 0);
}


void send_list(){
	printf("Client sending LIST\n");
	Message msg;
	msg.mtype = LIST;
	msg.id = client_id;
	msgsnd(server_queue, &msg, MSG_SIZE, 0);
}


void send_disconnect(){
	Message msg;
	msg.mtype = DISCONNECT;
	msg.id = client_id;
	msgsnd(server_queue, &msg, MSG_SIZE, 0);
	
	interlocutor_queue = -1;
}


void error_handle(char* message){
	
	fprintf(stderr, "An error occured in client: %s\n", message);
	raise(SIGINT);
}


void handle_message(Message* received){
	
	if(!received) return;
	
    switch(received->mtype){
            
        case CONNECT:
			handle_connect(received);
            break;
            
        case STOP:
			handle_stop(received);
            break;
            
        case LIST:
			handle_list(received);
            break;
        
        case CHAT:
			handle_chat(received);
			break;
		
		case DISCONNECT:
			handle_disconnect(received);
			break;
            
        default:
            break;
    }
	fflush(stdin);
}


void handle_connect(Message* received){
	printf("Client received CONNECT\n");
	if (sscanf(received->mtext, "%d", &interlocutor_queue) < 0)
		error_handle("Failed to register client");
	
	chat();
}


void handle_disconnect(Message* received){
	printf("Client received DISCONNECT\n");
	interlocutor_queue = -1;
}


void handle_stop(Message* received){
	printf("Client received STOP\n");
	send_stop();
}


void handle_list(Message* received){
	printf("Client received LIST\n");
	printf("%s\n", received->mtext);
}


void handle_chat(Message* received){
	printf("Answer: %s", received->mtext);
}


void send_message(char* message){
	Message msg;
	msg.mtype = CHAT;
	msg.id = client_id;
	sprintf(msg.mtext, "%s", message);
	msgsnd(interlocutor_queue, &msg, MSG_SIZE, 0);
}


void chat(){
	
	Message received;
	printf("\n\n=========== CHAT STARTED ==========\n");
	
	
	while(interlocutor_queue != -1){
		if (msgrcv(private_queue, &received, MSG_SIZE, -10, IPC_NOWAIT) >= 0)
            handle_message(&received);
       
       check_for_input();		
	}
}


void check_for_input(){
	char* line = NULL;
    size_t size = 0;

	if(input_given()){
		if(getline(&line, &size, stdin) != -1){
			
			if(strncmp("LIST", line, strlen("LIST")) == 0) {
				send_list();
				
			} else if (strncmp("CONNECT", line, strlen("CONNECT")) == 0) {
				char* token = strtok(line, " ");
				token = strtok(NULL, " ");
				int id = atoi(token);
				
				send_connect(id);
				
			} else if (strncmp("DISCONNECT", line, strlen("DISCONNECT")) == 0) {
				send_disconnect();
				
			} else if (strncmp("STOP", line, strlen("STOP")) == 0) {
				send_stop();
				
			}else if(interlocutor_queue != -1){
				send_message(line);
				
			}
		}
		
		fflush(stdin);
		fflush(stdout);
	}
}


int input_given(){
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	return (FD_ISSET(0, &fds));

}


int main(int argc, char** argv){
	
	init();
	
	printf("> ");
	fflush(stdout);
	Message received;
	
	 while(1){
		 
		 if (msgrcv(private_queue, &received, MSG_SIZE, -10, IPC_NOWAIT) >= 0)
            handle_message(&received);
            
		check_for_input();
    }
    
}
