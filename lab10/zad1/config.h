
#ifndef config_h
#define config_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <sys/epoll.h>

#define MAX_CLIENTS 12
#define MAX_MSG 256

void exit_error(char* error_msg) {
    perror(error_msg);
    exit(1);
}

typedef struct client {
  int fd;
  int active;
  int opponent;
  char* username;
} client;

client* clients[MAX_CLIENTS];

#endif //config_h
