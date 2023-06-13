#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/un.h>

#include <sys/epoll.h>

#include <fcntl.h>
#include <mqueue.h>

#include "shared.h"

//// ENUMS ////

typedef enum {
    EVENT_SOCKET, EVENT_CLIENT
} EventType;

//// DATA STRUCTURES ////

typedef struct {
    int socket;
    int client_id;
    EventType type;
} EventData;

typedef struct {
    char nick[MAX_NICKNAME_SIZE];
    int socket;
    bool is_active;
} ClientData;

//// GENERAL ////

bool global_is_running = true;

//// MUTEXES & THREADS////

static pthread_mutex_t mutex_client = PTHREAD_MUTEX_INITIALIZER;

//// CACHE ////

ClientData * client_sessions[MAX_CONNECTED_CLIENTS];

//// SOCKET DESCRIPTORS ////

int epoll_socket = 0;

int inet_socket = 0;
int local_socket = 0;

char * local_path;

//// SOCKET MANAGING ////

void remove_client(int client_id);
bool add_epoll_client( int sock_fd, int client_id );
bool add_epoll_socket( int sock_fd );
bool init_inet_connection( int port );
bool init_local_connection( const char * path );
bool init_socket_connection( const char * path, int port );

int is_connected_client( msg_buffer * message );
int check_connection(msg_buffer * message, int client_socket);

//// HANDLERS ////

bool handle_init( msg_buffer * message, int client_socket );
bool handle_list( msg_buffer * message, int client_socket );
bool handle_2all( msg_buffer * message, int client_socket );
bool handle_2one( msg_buffer * message, int client_socket );
bool handle_stop( msg_buffer * message, int client_socket );
bool handle_ping( msg_buffer * message, int client_socket );

void log_handled_message( msg_buffer * message );

bool handle_message( msg_buffer * message, int client_socket );

//// TERMINATION ////

void handle_exit();
void handle_sigint(int pid);
void * routine_ping (void * args);
void * handle_connections();

//// THREADS ////

pthread_t thread_ping;

//// MAIN ////

int main (int argc, char **argv) {

    if (argc < 3 ) {
        perror("Not enough arguments");
        return 1;
    }

    if (argc > 3) {
        perror("Too many arguments");
        return 2;
    }

    int port = atoi(argv[1]);
    local_path = argv[2];

    //// SETUP TERMINATION ////

    setup_exit_handler(handle_sigint);
    atexit(handle_exit);

    //// CREATE MESSAGE QUEUE ////

    if ( !init_socket_connection(local_path, port) ) {
        perror("Unable to create connection");
        return 3;
    }

    //// SETUP CLIENT QUEUES ////

    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        client_sessions[id] = NULL;
    }

    pthread_create(&thread_ping, NULL, routine_ping, NULL);    

    /// TODO: make it as thread ///
    handle_connections();
    

    return 0;
}



void remove_client(int client_id) {
    if ( client_sessions[client_id] == NULL)
    return;

    char content[MAX_MESSAGE_SIZE] = "Server stopped";

    msg_buffer message = create_message(
        E_STOP, 
        content, 
        SERVER_QUEUE_NAME, 
        client_id, 
        -1
    );

    send_message(&message, client_sessions[client_id]->socket);

    /// CLEANUP CLIENT SOCKETS ///

    shutdown(client_sessions[client_id]->socket, SHUT_RDWR);
    close(client_sessions[client_id]->socket);
    printf("[server]: removed:'%s' with id: %d\n", client_sessions[client_id]->nick, client_id);
    free(client_sessions[client_id]);

    client_sessions[client_id] = NULL;
}

bool add_epoll_client(
    int sock_fd,
    int client_id
) {
    struct epoll_event event;

    /// SETUP DATA ///
    EventData * event_data = calloc(1, sizeof( EventData ));
    event_data->client_id = client_id;
    event_data->type = EVENT_CLIENT;
    event_data->socket = sock_fd;
    
    /// SETUP EVENT ///
    event.events = EPOLLIN | EPOLLPRI;
    event.data.ptr = event_data;

    /// ADD TO EPOLL ///
    if (epoll_ctl(epoll_socket, EPOLL_CTL_ADD, sock_fd, &event) == -1) {
        perror("unable to add sock_fd to epoll");
        return false;
    }
    
    return true;   
}

bool add_epoll_socket(
    int sock_fd
) {
    struct epoll_event event;

    /// SETUP DATA ///
    EventData * event_data = calloc(1, sizeof( EventData ));
    event_data->client_id = -1;
    event_data->type = EVENT_SOCKET;
    event_data->socket = sock_fd;
    
    /// SETUP EVENT ///
    event.events = EPOLLIN | EPOLLPRI;
    event.data.ptr = event_data;

    /// ADD TO EPOLL ///
    if (epoll_ctl(epoll_socket, EPOLL_CTL_ADD, sock_fd, &event) == -1) {
        perror("unable to add sock_fd to epoll");
        return false;
    }
    
    return true;
}

bool init_inet_connection(
    int port
){
    struct sockaddr_in soc_server;
    inet_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (inet_socket == -1) {
        perror("[Error] at inet_socket: init");
        return false;
    }
        
    memset(&soc_server, 0, sizeof(soc_server));

    soc_server.sin_family = AF_INET;
    soc_server.sin_port = htons(port);
    soc_server.sin_addr.s_addr = htonl(INADDR_ANY); /// inet_addr("127.0.0.1")
    
    int reuse_flag = 1;
    if (
        setsockopt(
            inet_socket, 
            SOL_SOCKET, 
            SO_REUSEADDR, 
            &reuse_flag, 
            sizeof (reuse_flag)
        ) == -1
    ) {
        perror("[Error] at inet_socket: can't set reuse flag");
    }

    if (
        bind(
            inet_socket, 
            (struct sockaddr *) &soc_server, 
            sizeof soc_server
        ) == -1
    ) {
        perror("[Error] at inet_socket: bind");
        return false;
    }

    if (listen(inet_socket, MAX_CONNECTED_CLIENTS) == -1) {
        perror("[Error] at inet_socket: listen");
        return false;
    }

    return add_epoll_socket(inet_socket);
}

bool init_local_connection(
    const char * path
) {
    local_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (local_socket == -1) {
        perror("[Error] at local_socket: init");
        return false;
    }

    struct sockaddr_un soc_server;    
    soc_server.sun_family = AF_UNIX;
    strcpy(soc_server.sun_path, path);

    if (
        bind(
            local_socket, 
            (struct sockaddr *) &soc_server, 
            sizeof soc_server
        ) == -1
    ) {
        perror("[Error] at local_socket: bind");
        return false;
    }

    if (listen(local_socket, MAX_CONNECTED_CLIENTS) == -1) {
        perror("[Error] at inet_socket: listen");
        return false;
    }


    return add_epoll_socket(local_socket);
}

bool init_socket_connection(
    const char * path, 
    int port
) {
    epoll_socket = epoll_create1(0);

    if (epoll_socket == -1) {
        perror("[Error] at epoll_create1");
        return false;
    }

    if (!init_inet_connection(port)) {
        perror("[Error] at inet_socket");
        return false;
    }

    if(!init_local_connection(path)) {
        perror("[Error] at local_socket");
        return false;
    }

    return true;
}


int is_connected_client( msg_buffer * message ){
    return 
        message->client_id >= 0 && 
        message->client_id < MAX_CONNECTED_CLIENTS &&
        client_sessions[message->client_id] != NULL;
}

int check_connection(msg_buffer * message, int client_socket) {
    if (is_connected_client(message))
        return 1;

    char content[MAX_MESSAGE_SIZE] = "Connection failed: client id not found";

    msg_buffer response = create_message(
        message->command, 
        content,  
        message->mq_name,
        message->client_id, 
        -1
    );

    send_message(&response, client_socket);

    return 0;
}

//// HANDLERS ////

bool handle_init( msg_buffer * message, int client_socket) {
    int client_id = -1;

    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        if (client_sessions[id] == NULL) {
            client_id = id;
            
            /// SETUP CLIENT DATA STRUCTURE ///
            client_sessions[client_id] = calloc(1, sizeof ( ClientData ));
            client_sessions[client_id]->socket = client_socket;
            client_sessions[client_id]->is_active = true;
            strcpy(client_sessions[client_id]->nick,message->content);
            
            break;
        }
    }

    if (client_id == -1 || !add_epoll_client(client_socket, client_id)) {
        char content[MAX_MESSAGE_SIZE] = "Connection failed: too many connected clients";

        msg_buffer response = create_message(
            E_INIT, 
            content,  
            message->mq_name,
            client_id, 
            -1
        );

        send_message(&response, client_socket);
        return false;
    }

    char content[MAX_MESSAGE_SIZE] = "Connection success";

    msg_buffer response = create_message(
        E_INIT, 
        content,
        message->mq_name, 
        client_id, 
        -1
    );

    send_message(&response, client_socket);
    return true;
}

bool handle_list( msg_buffer * message, int client_socket ){
    if (!check_connection(message, client_socket)) return false;

    char buffer[MAX_MESSAGE_SIZE];

    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        if (client_sessions[id] != NULL) {
            char tmp_buff[MAX_BUFFER_SIZE];
            sprintf(tmp_buff, 
                id == message->client_id 
                ? "[%d] is you\n"
                : "[%d] is running as [%s]...\n", id, client_sessions[id]->nick);
            strcat(buffer, tmp_buff);
        }
    }

    msg_buffer response = create_message(
        E_LIST, 
        buffer, 
        message->mq_name,
        message->client_id, 
        -1
    );

    send_message(&response, client_socket);
    return true;
}

bool handle_2all( msg_buffer * message, int client_socket ){
    if (!check_connection(message, client_socket)) return false;
    
    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        if (client_sessions[id] != NULL && id != message->client_id) {
            printf("Send to %d as %s\n", id, client_sessions[id]->nick);
            
            msg_buffer tmp_msg = create_message(
                E_2ALL, 
                message->content, 
                message->mq_name,
                id, 
                message->client_id
            );
            
            strcpy(
                tmp_msg.other_nick, 
                client_sessions[message->client_id]->nick
            );

            send_message(&tmp_msg, client_sessions[id]->socket);
        }
    }

    return true;
}

bool handle_2one( msg_buffer * message, int client_socket ){
    if (!check_connection(message, client_socket)) return false;

    if (message->other_id < 0 || message->other_id >= MAX_BUFFER_SIZE)
        return false;

    if (client_sessions[message->other_id] == NULL)
        return false;

    int other_id = message->other_id;
    message->other_id = message->client_id;
    message->client_id = other_id;
    
    strcpy(
        message->other_nick, 
        client_sessions[message->other_id]->nick
    );

    send_message( message, client_sessions[other_id]->socket );
    return true;
}

bool handle_stop( msg_buffer * message, int client_socket ){
    if ( message->client_id < 0 || message->client_id >= MAX_CONNECTED_CLIENTS ) {
        return false;
    }

    remove_client(message->client_id);

    return true;
}

bool handle_ping(msg_buffer * message, int client_socket) {
    if (!check_connection(message, client_socket))
        return false;

    client_sessions[message->client_id]->is_active = true;

    return 0;
}

void log_handled_message( msg_buffer * message ) {
    FILE * file = NULL;

    file = fopen("log.tmp", "a");
    
    if (file == NULL) {
        perror("unable to open file");
        return;
    }

    time_t curr_time = time(NULL);
    struct tm time_struct = *localtime(&curr_time);

    fprintf(
        file, 
        "[%02d:%02d:%02d](%s) %s\n", 
        time_struct.tm_hour, 
        time_struct.tm_min, 
        time_struct.tm_sec,
        command_to_string(message->command),
        message->content
    );

    fclose(file);
}

bool handle_message( msg_buffer * message, int client_socket ) {
    pthread_mutex_lock(&mutex_client);

    bool is_success = true;

    switch (message->command) {
        case E_INIT: is_success = handle_init(message, client_socket); break;
        case E_LIST: is_success = handle_list(message, client_socket); break;
        case E_2ALL: is_success = handle_2all(message, client_socket); break;
        case E_2ONE: is_success = handle_2one(message, client_socket); break;
        case E_STOP: is_success = handle_stop(message, client_socket); break;
        case E_PING: is_success = handle_ping(message, client_socket); break;
        default:     is_success = false;;
    }

    printf("received: %s : %s\n", command_to_string(message->command), message->content);
    log_handled_message(message);

    pthread_mutex_unlock(&mutex_client);

    return is_success;
}


//// TERMINATION ////

void handle_exit() {
    pthread_mutex_lock(&mutex_client);
    global_is_running = false;
    pthread_mutex_unlock(&mutex_client);


    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        remove_client(id);
    }

    /// CLEANUP SERVER SOCKETS ///

    shutdown(inet_socket, SHUT_RDWR);
    shutdown(local_socket, SHUT_RDWR);

    remove(local_path);

    printf("server exit\n");
}

void handle_sigint(int pid) {
    printf("terminated by process : %d\n", pid);
    exit(0);
}

void * routine_ping (void * args) {

    bool is_running;

    do {
        printf("ping started\n");

        for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {

            /// PING CLIENT ///
            pthread_mutex_lock(&mutex_client);
            if (client_sessions[id] != NULL) {
                msg_buffer message;
                message.command = E_PING;
                send_message(&message, client_sessions[id]->socket);
                printf("ping %s with id: %d\n", client_sessions[id]->nick, id);
                client_sessions[id]->is_active = false;
            }
            pthread_mutex_unlock(&mutex_client);

            sleep(1);

            /// REMOVE INACTIVE CLIENT ///
            pthread_mutex_lock(&mutex_client);
            if (
                client_sessions[id] != NULL && 
                !client_sessions[id]->is_active
            ){
                remove_client(id);
            }
            pthread_mutex_unlock(&mutex_client);
        }

        pthread_mutex_lock(&mutex_client);
        is_running = global_is_running;
        pthread_mutex_unlock(&mutex_client);

    } while(is_running);

    return 0;
}

void * handle_connections() {
    struct epoll_event events[MAX_CONNECTED_CLIENTS];

    bool is_running;

    do {
        int readed_fds = epoll_wait(epoll_socket, events, MAX_CONNECTED_CLIENTS, 10);

        for (int i = 0; i < readed_fds; i++) {
            EventData * event_data = events[i].data.ptr;

            if ( event_data->type == EVENT_SOCKET ) {

                /// ACCEPT INCOMMING CONNECTION ///
                struct sockaddr_in soc_client;
                int soc_length = sizeof (soc_client);

                int client_socket = accept(
                    event_data->socket,
                    (struct sockaddr *) &soc_client,
                    (socklen_t *) &soc_length
                );

                if (client_socket == -1) {
                    perror("[Error] at accepting new connection");
                    continue;
                }

                /// SETUP NEW CLIENT ///
                msg_buffer message;
                read(client_socket, &message, MAX_MESSAGE_BUFFER_SIZE);

                if (!handle_message(&message, client_socket)) {
                    perror("Unable to handle message");
                    continue;
                }
                
            }

            if (event_data->type == EVENT_CLIENT) {
                
                ClientData * client_data = client_sessions[event_data->client_id];
                
                msg_buffer message_buffer;
                
                printf("incoming %d\n", i);

                read(client_data->socket, &message_buffer, MAX_MESSAGE_BUFFER_SIZE);        
                handle_message(&message_buffer, client_data->socket);
            }
        }

        pthread_mutex_lock(&mutex_client);
        is_running = global_is_running;
        pthread_mutex_unlock(&mutex_client);

    } while(is_running);

    return 0;
}