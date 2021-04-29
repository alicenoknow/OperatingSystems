#include "config.h"


// Headers
void init();
void remove_queue();
void server_queue_setup();
void clients_slots_setup();
void signals_handling_setup();
void sigint_handler(int signum);
void run();
void handle_message(Message* received);
void handle_init(Message* received);
void handle_connect(Message* received);
void error_handle(char* message);
void connect_client(int client_id, int interlocutor_id, int client_queue, int interlocutor_queue);
void handle_disconnect(Message* received);
void disconnect_client(int client_id);
void handle_stop(Message* received);
void remove_client(int client_id);
void handle_list(Message* received); 


//Globals
key_t server_key;
int server_queue = -1;
Client* clients;
int clients_no = 0;
 

void remove_queue(){
	if(server_queue != -1) 
		msgctl(server_queue, IPC_RMID, NULL); 
}


void init(){
	
	server_queue_setup();
	clients_slots_setup();
	signals_handling_setup();

}


void server_queue_setup(){
	
	char *path = getenv("HOME");
	if(path == NULL)
		error_handle("Failed to get enviromental variable HOME");
		
	server_key = ftok(path, PROJ_ID);
	if(server_key == -1)
		error_handle("Failed to create server key");
		
	server_queue = msgget(server_key, 0666 | IPC_CREAT );  // 0666 - permissions, IPC_CREAT - create if does not exist, IPC_EXCL - error if already exists
	if(server_queue == -1)
		error_handle("Failed to create server queue");
	atexit(remove_queue);
}


void clients_slots_setup(){
	
	clients = calloc(MAX_CLIENTS, sizeof(Client));
	for(int i = 0; i < MAX_CLIENTS; i++){
		Client new_client;
		new_client.id = -1;
		new_client.status = INACTIVE;
		new_client.queue = -1;
		new_client.interlocutor_id = -1;
		
		clients[i] = new_client;
	}
}


void signals_handling_setup(){
	
	struct sigaction act;
    act.sa_flags = 0;
    sigfillset(&act.sa_mask);
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);
}


void sigint_handler(int signum){
	
	 for(int i = 0; i < MAX_CLIENTS; i++)
        if(clients[i].id != -1) {
            Message msg;
            msg.mtype = STOP;
            msgsnd(clients[i].queue, &msg, 0, 0);
        }
    
    Message buf;
    while (clients_no > 0) {
        if (msgrcv(server_queue, &buf, MSG_SIZE, -10, 0) >= 0)
			handle_stop(&buf);
    }
	exit(0);
}


void run(){
	
	Message new_message;
	
	while(1){
			
		if (msgrcv(server_queue, &new_message, MSG_SIZE, -10, 0) < 0)
            error_handle("Failed to read new message");
		
		handle_message(&new_message);
	}
	
}


void handle_message(Message* received){
	
	if (received == NULL) return;
	
    switch(received->mtype){
        case INIT:
			handle_init(received);
            break;
            
        case CONNECT:
			handle_connect(received);
            break;
            
        case DISCONNECT:
			handle_disconnect(received);
            break;
            
        case STOP:
			handle_stop(received);
            break;
            
        case LIST:
			handle_list(received);
            break;
            
        default:
            break;
    }

}


void handle_init(Message* received) {
	printf("Server received INIT\n");
	
	key_t new_client_key;
	int client_queue;
	
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].status == INACTIVE) {
			
			if (sscanf(received->mtext, "%d", &new_client_key) < 0)
				error_handle("Failed to read new client key");
			
			printf("SERVER %d\n", new_client_key);
			client_queue = msgget(new_client_key, 0);
			if (client_queue == -1)
				error_handle("Failed to open new client queue");
			
            clients[i].id = i;
            clients[i].status = ACTIVE;
            clients[i].queue = client_queue;
            
            Message server_reply;
            server_reply.mtype = INIT;
            server_reply.id = i;
            sprintf(server_reply.mtext, "%d", i);
            
            if(msgsnd(clients[i].queue, &server_reply, MSG_SIZE, 0) == -1)
				error_handle("Failed to answer INIT message");
            
            clients_no++;
			return;
        }
    }
    error_handle("Failed to init new client. Maximum number of clients reached");
}


void handle_connect(Message* received) {
	printf("Server received CONNECT\n");
	int first_client_id = received->id;
	int second_client_id;
	
	if(sscanf(received->mtext, "%d", &second_client_id) < 0)
		error_handle("Failed to read second client id to connect");
	
	if(clients[second_client_id].status != ACTIVE)
		error_handle("Client to connect is not active");
	
	int first_queue = clients[first_client_id].queue;
	int second_queue = clients[second_client_id].queue;
	
	connect_client(first_client_id, second_client_id, first_queue, second_queue);
	connect_client(second_client_id, first_client_id, second_queue,  first_queue);

}


void connect_client(int client_id, int interlocutor_id, int client_queue, int interlocutor_queue){
	
	Message server_reply;
	server_reply.mtype = CONNECT;
	server_reply.id = client_id;
	sprintf(server_reply.mtext, "%d", interlocutor_queue);
	
	if(msgsnd(client_queue, &server_reply, MSG_SIZE, 0) == -1)
		error_handle("Failed to send first CONNECT message");
		
	clients[client_id].status = OCCUPIED;
	clients[client_id].interlocutor_id = interlocutor_id;
}


void handle_disconnect(Message* received) {
	printf("Server received DISCONNECT\n");
	int first_client_id = received->id;
	int second_client_id = clients[first_client_id].interlocutor_id;
	
	
	disconnect_client(first_client_id);
	disconnect_client(second_client_id);
}


void disconnect_client(int client_id){
	
	if(clients[client_id].status != OCCUPIED)
		return;
	
	Message server_reply;
	server_reply.mtype = DISCONNECT;
	server_reply.id = client_id;
	if(msgsnd(clients[client_id].queue, &server_reply, MSG_SIZE, 0) == -1)
		error_handle("Failed to send first DISCONNECT message");
		
	clients[client_id].status = ACTIVE;
	clients[client_id].interlocutor_id = -1;
	
}


void handle_stop(Message* received) {
	printf("Server received STOP\n");
	int client_id = received->id;
	remove_client(client_id);
}


void remove_client(int client_id){

	clients[client_id].interlocutor_id = -1;
	clients[client_id].id = -1;
	clients[client_id].status = INACTIVE;
	clients[client_id].queue = -1;
	clients_no--;
}


void handle_list(Message* received) {
	printf("Server received LIST\n");
	int client_id = received->id;
	char* reply = calloc(MAX_MSG_LEN, sizeof(char));
	char* buff = calloc(20, sizeof(char));
	
	strcpy(reply, "\n\n========== List of available clients ==========\n");
	
	for (int i = 0; i < MAX_CLIENTS; i++)
        if(clients[i].id != -1) {
			sprintf(buff,"\nClient: %d status:", i);
			strcat(reply, buff);
			
			if(clients[i].status == ACTIVE)
				strcat(reply, " active");
			
			else 
				strcat(reply, " occupied");
			
		}
	
	strcat(reply, "\n\0");
	
	Message server_reply;
    server_reply.mtype = LIST;
	sprintf(server_reply.mtext, "%s", reply);
    server_reply.id = client_id;
    
    if(msgsnd(clients[client_id].queue, &server_reply, MSG_SIZE, 0) == -1)
		error_handle("Failed to answer LIST message");
		
    free(buff);
    free(reply);
}


void error_handle(char* message){
	
	fprintf(stderr, "An error occured in server: %s\n", message);
	printf("ERRNO %s\n", strerror(errno));
	
	raise(SIGINT);

}


int main(int argc, char** argv){
	
	init();
	run();
	
	return 0;
}
