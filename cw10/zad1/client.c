#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/un.h>
#include <sys/epoll.h>

#define MAX_EPOLL_EVENTS    2
#define MAX_EPOLL_TIMEOUT   10

#include "shared.h"

//// UTILS ////

const char * help_message = "<nick> <[local/inet]> <server ip> <server port>";

int  server_closed = 0;

//// GENERAL ////

char client_nickname[MAX_NICKNAME_SIZE];
int server_socket = 0;
int server_epoll = 0;

int client_id = -1;

//// MUTEXES & THREADS ////

pthread_mutex_t mutex_socket = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_receive = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread_receive;

//// SOCKETS ////

typedef enum {
    ST_INET, ST_LOCAL, ST_UNKNOWN
} SocketType;

//// MESSAGE UTILS ////

msg_buffer c_create_message(command cmd, char content[MAX_MESSAGE_SIZE]) {
    pthread_mutex_lock(&mutex_socket);
    msg_buffer message =  create_message(cmd, content, "", client_id, -1);
    pthread_mutex_unlock(&mutex_socket);
    
    return message;
}

void c_send_message( msg_buffer * message ) {
    pthread_mutex_lock(&mutex_socket);
    send_message( message, server_socket );
    pthread_mutex_unlock(&mutex_socket);
}

//// RESPONSE HANLDING ////

bool is_client_active = true;

void handle_receive(int sock) {
    pthread_mutex_lock(&mutex_receive);
    msg_buffer * message = calloc(1, sizeof(msg_buffer));

    read(sock, message, MAX_MESSAGE_BUFFER_SIZE);

    switch (message->command) {
        case E_2ONE:
            printf(
                "[%02d:%02d:%02d] ", 
                message->time.tm_hour, 
                message->time.tm_min, 
                message->time.tm_sec
            );
        case E_2ALL:
            printf("from: [%d][%s]:\n", message->other_id, message->other_nick);
        case E_LIST:
            printf("%s\n", message->content);
            break;
        case E_STOP:
            free(message);
            server_closed = 1;
            pthread_mutex_unlock(&mutex_receive);
            exit(0);
            break;
        case E_PING:
            msg_buffer message = c_create_message(E_PING, "PING");
            c_send_message(&message);
            break;
        default:
            break;
    }

    free(message);

    pthread_mutex_unlock(&mutex_receive);
}

void check_server_response() {
    struct epoll_event events[MAX_EPOLL_EVENTS];

    int readed_fds = epoll_wait(server_epoll, events, MAX_EPOLL_EVENTS, MAX_EPOLL_TIMEOUT);

    for (int i = 0; i < readed_fds; i++ ) {
        handle_receive(events[i].data.fd);
    }
}

void * routine_receive( void * args  ){
    bool is_active;

    do {
        check_server_response();

        /// AVOID RACE CONDITION ///
        pthread_mutex_lock(&mutex_receive);
        is_active = is_client_active;  
        pthread_mutex_unlock(&mutex_receive);
    } while (is_active);

    return 0;
}

//// TERMINATION ////

void handle_exit() {
    pthread_mutex_lock(&mutex_receive);
    is_client_active = false;
    pthread_mutex_unlock(&mutex_receive);

    pthread_join(thread_receive, NULL);

    msg_buffer message = c_create_message(E_STOP, "stop");
    if (server_closed == 0) {
        c_send_message(&message);
        read(server_socket, &message, MAX_MESSAGE_BUFFER_SIZE);
    }

    close(server_socket);

    printf("client exit\n");
}

void handle_sigint(int pid) {
    printf("terminated by process : %d\n", pid);
    exit(0);
}

bool init_local_socket(
    const char * path 
) {
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if ( server_socket == -1 ) {
        perror("[Error] at socket: init");
        return false;
    }

    struct sockaddr_un soc_server;
    soc_server.sun_family = AF_UNIX;
    strcpy(soc_server.sun_path, path);

    if (
        connect(
            server_socket, 
            (struct sockaddr *) &soc_server, 
            sizeof (soc_server)
        ) == -1
    ) {
        perror("[Error] at socket: connect");
        return false;
    }

    return true;
}

bool init_inet_socket(
    const char * address,
    const int port
){
    struct sockaddr_in soc_server;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
        perror("[Error] at socket: init");
        return false;
    }

    memset(&soc_server, 0, sizeof(soc_server));
    soc_server.sin_family = AF_INET;
    soc_server.sin_port = htons(port);
    soc_server.sin_addr.s_addr = inet_addr(address);

    if (
        connect(
            server_socket,
            (struct sockaddr *) &soc_server,
            sizeof soc_server
        ) < 0
    ) {
        perror("Error at socket: connect");
        return false;
    }

    return true;
}

bool setup_epoll(int sock) {
    if (server_epoll == 0) {
        server_epoll = epoll_create1(0);
    }
    
    if (server_epoll == -1) {
        perror("[Error] at setup_epoll: init");
        return false;    
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = sock;

    if (epoll_ctl(server_epoll, EPOLL_CTL_ADD, sock, &event) == -1) {
        perror("[Error] at setup_epoll: epoll_ctl");
        return false;
    }

    return true;
}

//// MAIN ////

int main (int argc, char ** argv) {

    if (argc < 4) {
        perror("Not enough arguments");
        printf("%s %s\n", argv[0], help_message);
        return 1;
    }

    if (argc > 5) {
        perror("Too many arguments");
        printf("%s %s\n", argv[0], help_message);
        return 1;
    }

    if (strlen(argv[1]) >= MAX_NICKNAME_SIZE) {
        perror("Too long nickname");
        return 2;
    }

    strcpy(client_nickname, argv[1]);

    char * connection_socket = argv[2]; // local or inet
    char * server_address    = argv[3];

    int server_port          = atoi(argv[4]);

    SocketType socket_type = ST_UNKNOWN;

    if (strcmp(connection_socket, "inet") == 0) {
        socket_type = ST_INET;
    }

    if (strcmp(connection_socket, "local") == 0) {
        socket_type = ST_LOCAL;
    }

    if (socket_type == ST_UNKNOWN) {
        perror("[Error] invalid socket type");
        printf("%s %s\n", argv[0], help_message);
        return 4;
    }

    //// VALIDATION ////

    if (socket_type == ST_INET && argc != 5) {
        perror("[Error] invalid number of arguments");
        printf("%s %s\n", argv[0], help_message);
        return 3;
    }

    if (socket_type == ST_LOCAL && argc != 4) {
        perror("[Error] invalid number of arguments");
        printf("%s %s\n", argv[0], help_message);
        return 3;
    }

    //// SETUP SOCKET ////

    if (
        socket_type == ST_INET && 
        !init_inet_socket(server_address, server_port)
    ) {
        perror("[Error] failed to create inet socket");
        return 5;
    }

    if (
        socket_type == ST_LOCAL && 
        !init_local_socket(server_address)
    ) {
        perror("[Error] failed to create local socket");
        return 5;
    }

    setup_epoll(server_socket);

    //// SETUP TERMINATION ////

    setup_exit_handler(handle_sigint);
    atexit(handle_exit);

    //// SEND INIT COMMAND ////

    msg_buffer message_buffer = c_create_message(E_INIT, client_nickname);
    c_send_message(&message_buffer);

    read(server_socket, &message_buffer, sizeof(message_buffer));

    client_id = message_buffer.client_id;

    if (client_id == -1) {
        printf("This server is busy\n");
        return 0;
    }

    //// THREADS ////

    pthread_create(&thread_receive, NULL, routine_receive, NULL );

    //// REPL ////

    int repl_active = 1;
    
    while (repl_active) {
        char * buffer = NULL;
        size_t buff_len;

        int readed = getline(&buffer, &buff_len, stdin);
        buffer[readed - 1] = 0;

        char * ptr = strtok(buffer, DELIMITERS);

        if (ptr == NULL)
            continue;

        char *  cmd_str = ptr;
        command cmd = string_to_command(cmd_str);

        msg_buffer message = c_create_message(cmd, "");

        switch (cmd) {
            case E_2ONE:;

                ptr = strtok(NULL, DELIMITERS);

                if (ptr == NULL) {
                    printf("not enough arguments\n");
                    continue;
                }

                message.other_id = atoi(ptr);

            case E_2ALL:;
                ptr = strtok(NULL, "");
                
                if (ptr == NULL) {
                    printf("empty content not allowed!\n");
                    continue;
                }
                
                set_content(&message, ptr);
                c_send_message(&message);
                break;

            case E_LIST:
                c_send_message(&message);
                break;

            case E_STOP:
                repl_active = 0;
                break;
        
            default:
                printf("Unknown command: '%s'\n", cmd_str);
                continue;
                break;

            sleep(1);
        }

        if (buffer != NULL)
            free(buffer);

    }


    return 0;
}