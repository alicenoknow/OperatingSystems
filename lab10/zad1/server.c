#include "config.h"

int local_socket, network_socket, active_clients;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


void init_local_socket(char* path){
    local_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un local_sockaddr;
    memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, path);

    unlink(path);
    bind(local_socket, (struct sockaddr*)&local_sockaddr, sizeof(struct sockaddr_un));
    listen(local_socket, MAX_CLIENTS);
}


void init_network_socket(int port) {
    struct sockaddr_in web_addr = {
        .sin_family = AF_INET, .sin_port = htons(port),
        .sin_addr = { .s_addr = htonl(INADDR_ANY) },
    };

    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    bind(network_socket, (struct sockaddr*)&web_addr, sizeof(struct sockaddr_in));
    listen(network_socket, MAX_CLIENTS);
}

int find_client(char* name) {
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && !strcmp(name, clients[i]->username))
            return i;
    return -1;
}

void remove_client(char* username) {
    printf("Removing client: %s\n", username);
    int client_id = find_client(username);
    int opponent_id;
    if (client_id == -1) return;

    opponent_id = clients[client_id]->opponent;
    free(clients[client_id]->username);
    free(clients[client_id]);
    clients[client_id] = NULL;
    active_clients--;

    if (clients[opponent_id] != NULL) {
        free(clients[opponent_id]->username);
        free(clients[opponent_id]);
        clients[opponent_id] = NULL;
        active_clients--;
    }
}


int check_sockets(int local_socket, int network_socket) {
    struct pollfd* pollstruct = calloc(2 + active_clients, sizeof(struct pollfd));
    pollstruct[0].fd = local_socket;
    pollstruct[0].events = POLLIN;
    pollstruct[1].fd = network_socket;
    pollstruct[1].events = POLLIN;

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < active_clients; i++) {
        pollstruct[i + 2].fd = clients[i]->fd;
        pollstruct[i + 2].events = POLLIN;
    }
    pthread_mutex_unlock(&clients_mutex);

    poll(pollstruct, active_clients + 2, -1);
    int val;
    for (int i = 0; i < active_clients + 2; i++)
        if (pollstruct[i].revents & POLLIN) {
            val = pollstruct[i].fd;
            break;
        }

    if (val == local_socket || val == network_socket) 
        val = accept(val, NULL, NULL);

    free(pollstruct);
    return val;
}


void ping_loop() {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && !clients[i]->active) {
            remove_client(clients[i]->username);
        }

    for (int i = 0; i < MAX_CLIENTS; i++) 
        if (clients[i] != NULL) {
            send(clients[i]->fd, "ping| ", MAX_MSG, 0);
            clients[i]->active = 0;
        }
    
    pthread_mutex_unlock(&clients_mutex);
    sleep(5);
    ping_loop();
}


int find_opponent(int current) {
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (i != current && clients[i] != NULL && clients[i]->opponent == -1)
            return i;
    return -1;
}

int new_client(char* username, int client_fd) {
    if (find_client(username) != -1) return -1;

    int index = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] == NULL) {
            index = i;
            break;
        }

    if (index != -1) {
        client* new_client = calloc(1, sizeof(client));
        new_client->username = calloc(15, sizeof(char));
        strcpy(new_client->username, username);
        new_client->fd = client_fd;
        new_client->active = 1;
        new_client->opponent = -1;

        clients[index] = new_client;
        active_clients++;
    }
    return index;
}

void server_loop() {
    while(1) {
        int client_fd = check_sockets(local_socket, network_socket);

        char buffer[MAX_MSG + 1];
        recv(client_fd, buffer, MAX_MSG, 0);

        char* cmd = strtok(buffer, "|");
        char* args = strtok(NULL, "|");
        char* username = strtok(NULL, "|");

        pthread_mutex_lock(&clients_mutex);
        if (strcmp(cmd, "new") == 0) {
            int index = new_client(username, client_fd);

            if (index == -1) {
                send(client_fd, "start|taken", MAX_MSG, 0);
                close(client_fd);

            } else {
                int opponent = find_opponent(index);

                if (opponent != -1) {
                    send(clients[opponent]->fd, "start|O", MAX_MSG, 0);
                    send(clients[index]->fd, "start|X", MAX_MSG, 0);
                    clients[opponent]->opponent = index;
                    clients[index]->opponent = opponent;
                }
                else {
                    send(clients[index]->fd, "start|wait", MAX_MSG, 0);
                }
            }
        }
        if (strcmp(cmd, "move") == 0) {
            int move = atoi(args);
            int player = find_client(username);
            sprintf(buffer, "move|%d", move);
            send(clients[clients[player]->opponent]->fd, buffer, MAX_MSG, 0);
        }
        if (strcmp(cmd, "disconnect") == 0) {
            remove_client(username);
        }
        if (strcmp(cmd, "pong") == 0) {
            int player = find_client(username);
            if (player != -1) {
                clients[player]->active = 1;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}

void sig_handler(int signo) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL) {
            if (send(clients[i]->fd, "disconnect|", MAX_MSG, 0) == -1) 
                exit_error("Error while disconnecting player!\n");
            remove_client(clients[i]->username);
        }
    }
    close(network_socket);
    close(local_socket);
    printf("Server closed successfully...\n");
    exit(0);
}

int main(int argc, char** argv) {
    if (argc < 3)
        exit_error("Invalid number of arguments!\n");

    int port = atoi(argv[1]);
    char* socket_path = argv[2];

    signal(SIGINT, sig_handler);

    init_local_socket(socket_path);
    init_network_socket(port);

    pthread_t ping;
    pthread_create(&ping, NULL, (void* (*)(void*))ping_loop, NULL);

    server_loop();
}
