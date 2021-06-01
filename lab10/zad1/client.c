#include "config.h"


int server_socket;
int is_o;
char buffer[MAX_MSG + 1];
char* username;

int board[9];

typedef enum {
    START,
    WAIT_FOR_ENEMY,
    WAIT_FOR_MOVE,
    ENEMY_MOVE,
    MOVE,
    DISCONNECT
} Stage;
Stage stage = START;

char *cmd, *arg;

pthread_mutex_t reply_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reply_cond = PTHREAD_COND_INITIALIZER;

char symbols[2] = {'O', 'X'};

int make_move(int position, int this_move) {
    if (position < 0 || position > 9 || board[position] >= 0) return 0;
	if (this_move == 1) {
    	board[position] =  is_o ? 0 : 1;
	} else {
		board[position] =  is_o ? 1 : 0;
	}
    return 1;
}

char full_column() {
    for (int x = 0; x < 3; x++) {
        int first = board[x];
        int second = board[x + 3];
        int third = board[x + 6];
        if (first == second && second == third && first >= 0) return symbols[first];
    }
    return '-';
}

char full_row() {
    for (int y = 0; y < 3; y++) {
        int first = board[3 * y];
        int second = board[3 * y + 1];
        int third = board[3 * y + 2];
        if (first == second && first == third && first >= 0) return symbols[first];
    }
    return '-';
}

char full_diagonal() {
    int first = board[0];
    int second = board[4];
    int third = board[8];
    if (first == second && first == third && first >= 0) return symbols[first];

    first = board[2];
    second = board[4];
    third = board[6];
    if (first == second && first == third && first >= 0) return symbols[first];

    return '-';
}

char get_winner() {
    char column = full_column(board);
    char row = full_row(board);
    char diagonal = full_diagonal(board);
    return column != '-' ? column : row != '-' ? row : diagonal;
}

void disconnect() {
    char buffer[MAX_MSG + 1];
    sprintf(buffer, "disconnect| |%s", username);
    send(server_socket, buffer, MAX_MSG, 0);
    exit(0);
}

void has_game_ended() {
    int has_won = 0;
    char winner = get_winner();
    if (winner != '-') {
        if ((is_o && winner == 'O') || (!is_o && winner == 'X')) {
            printf("Winner! :3\n");
        } else {
            printf("Loser :c\n");
        }
        has_won = 1;
    }

    int is_draw = 1;
    for (int i = 0; i < 9; i++)
        if (board[i] < 0) {
            is_draw = 0;
            break;
        }

    if (is_draw && !has_won) 
        printf("It's a draw!\n");
    
    if (has_won || is_draw) 
        stage = DISCONNECT;
}

void read_reply(char* reply) {
    cmd = strtok(reply, "|");
    arg = strtok(NULL, "|");
}

void print_board() {
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) 
            printf("|%d|", board[y * 3 + x]);
        
        printf("\n____________\n");
	}
}

void loop() {
    printf("Args: %s\n", arg);
    if (stage == START) {
        if (strcmp(arg, "taken") == 0) {
            exit_error("Invalid username!\n");
            
        } else if (strcmp(arg, "wait") == 0) {
            printf("Waiting for another player to join...\n");
            stage = WAIT_FOR_ENEMY;
        } else {
            is_o = (arg[0] == 'O');
            stage = is_o ? MOVE : WAIT_FOR_MOVE;
        }
    } else if (stage == WAIT_FOR_ENEMY) {

        pthread_mutex_lock(&reply_mutex);
        while (stage != START && stage != DISCONNECT)
            pthread_cond_wait(&reply_cond, &reply_mutex);
        
        pthread_mutex_unlock(&reply_mutex);

        is_o = arg[0] == 'O';
        stage = is_o ? MOVE : WAIT_FOR_MOVE;

    } else if (stage == WAIT_FOR_MOVE) {
        printf("Waiting for enemy's move...\n");

        pthread_mutex_lock(&reply_mutex);
        while (stage != ENEMY_MOVE && stage != DISCONNECT)
            pthread_cond_wait(&reply_cond, &reply_mutex);
        pthread_mutex_unlock(&reply_mutex);

    } else if (stage == ENEMY_MOVE) {
        int move = atoi(arg);
        make_move(move, 0);
        has_game_ended();
        if (stage != DISCONNECT)
            stage = MOVE;

    } else if (stage == MOVE) {
        print_board();

        int move;
        do {
            printf("Your move (%c): ", is_o ? 'O' : 'X');
            scanf("%d", &move);
        } while (!make_move(move-1, 1));

        print_board();

        char buffer[MAX_MSG + 1];
        sprintf(buffer, "move|%d|%s", move-1, username);
        send(server_socket, buffer, MAX_MSG, 0);

        has_game_ended();
        if (stage != DISCONNECT) {
            stage = WAIT_FOR_MOVE;
        }
    } else if (stage == DISCONNECT) {
        disconnect();
    }
    loop();
}

int main(int argc, char* argv[]) {
    if (argc < 4) 
		exit_error("Invalid number of arguments!\n");

    username = argv[1];
    char* type = argv[2];
    char* destination = argv[3];

    signal(SIGINT, disconnect);

	for (int i = 0; i < 9; i++)
		board[i] = -1;

    if (strcmp(type, "local") == 0) {
        server_socket = socket(AF_UNIX, SOCK_STREAM, 0);

        struct sockaddr_un local_sockaddr;
        memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
        local_sockaddr.sun_family = AF_UNIX;
        strcpy(local_sockaddr.sun_path, destination);

        connect(server_socket, (struct sockaddr*)&local_sockaddr,
                sizeof(struct sockaddr_un));
    } else {
        struct addrinfo* info;
        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        getaddrinfo("network", destination, &hints, &info);

        server_socket =
            socket(info->ai_family, info->ai_socktype, info->ai_protocol);

        connect(server_socket, info->ai_addr, info->ai_addrlen);

        freeaddrinfo(info);
    }
    char buffer[MAX_MSG+ 1];
    sprintf(buffer, "new| |%s", username);
    send(server_socket, buffer, MAX_MSG, 0);

    int running = 0;

    while (1) {
        recv(server_socket, buffer, MAX_MSG, 0);
        read_reply(buffer);
        printf("Message: %s\n", buffer);
        pthread_mutex_lock(&reply_mutex);

        if (strcmp(cmd, "start") == 0) {
            stage = START;
            if (!running) {
                pthread_t game;
                pthread_create(&game, NULL, (void* (*)(void*))loop, NULL);
                running = 1;
            }

        } else if (strcmp(cmd, "move") == 0) {
            stage = ENEMY_MOVE;

        } else if (strcmp(cmd, "disconnect") == 0) {
            stage = DISCONNECT;
            exit(0);

        } else if (strcmp(cmd, "ping") == 0) {
            sprintf(buffer, "pong| |%s", username);
            send(server_socket, buffer, MAX_MSG, 0);

        }

        pthread_cond_signal(&reply_cond);
        pthread_mutex_unlock(&reply_mutex);
    }
}