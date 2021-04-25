
#ifndef config_h
#define config_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>


// Limits
#define MAX_CLIENTS 10
#define MAX_MSG_LEN 200


// Commands
#define STOP 1
#define DISCONNECT 2
#define LIST 3
#define CONNECT 4
#define INIT 5
#define CHAT 6

// Client status
#define INACTIVE -1
#define ACTIVE 1
#define OCCUPIED 2

// Server ID
#define PROJ_ID 's'


// Message
typedef struct Message{
	
    long mtype;
    int id;
    char mtext[MAX_MSG_LEN];
    
}Message;

// Client
typedef struct Client{
	
    int id;
    int status;
    int queue;
    int interlocutor_id;
    
}Client;


// Message size
const size_t MSG_SIZE = sizeof(Message) - sizeof(long);


#endif //config_h
