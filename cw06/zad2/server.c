#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>

#include "shared.h"

//// CACHE ////

mqd_t server_queue;

mqd_t client_queues[MAX_CONNECTED_CLIENTS];

int is_connected_client(msg_buffer * message ){
    return 
        message->client_id >= 0 && 
        message->client_id < MAX_CONNECTED_CLIENTS &&
        client_queues[message->client_id] != NULL;
}

int check_connection(msg_buffer * message) {
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

    send_message(&response, mq_open(message->mq_name, O_RDWR));

    return 0;
}

//// HANDLERS ////

void handle_init( msg_buffer * message ) {
    int client_id = -1;

    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        if (client_queues[id] == NULL) {
            client_id = id;
            client_queues[client_id] = mq_open(message->mq_name, O_RDWR);
            break;
        }
    }

    if (client_id == -1) {
        char content[MAX_MESSAGE_SIZE] = "Connection failed: too many connected clients";

        msg_buffer response = create_message(
            E_INIT, 
            content,  
            message->mq_name,
            client_id, 
            -1
        );

        send_message(&response, mq_open(message->mq_name, O_RDWR));
        return;
    }

    char content[MAX_MESSAGE_SIZE] = "Connection success";

    msg_buffer response = create_message(
        E_INIT, 
        content,
        message->mq_name, 
        client_id, 
        -1
    );

    send_message(&response, client_queues[client_id]);
}

void handle_list( msg_buffer * message ){
    if (!check_connection(message)) return;

    char buffer[MAX_MESSAGE_SIZE];

    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        if (client_queues[id] != NULL) {
            char tmp_buff[MAX_BUFFER_SIZE];
            sprintf(tmp_buff, 
                id == message->client_id 
                ? "[%d] is you\n"
                : "[%d] is running...\n", id);
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

    send_message(&response, client_queues[message->client_id]);
}

void handle_2all( msg_buffer * message ){
    if (!check_connection(message)) return;
    
    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        if (client_queues[id] != NULL && id != message->client_id) {
            printf("Send to %d\n", id);
            msg_buffer tmp_msg = create_message(
                E_2ALL, 
                message->content, 
                message->mq_name,
                id, 
                message->client_id
            );
            send_message(&tmp_msg, client_queues[id]);
        }
    }
}

void handle_2one( msg_buffer * message ){
    if (!check_connection(message)) return;

    if (message->other_id < 0 || message->other_id >= MAX_BUFFER_SIZE)
        return;

    if (client_queues[message->other_id] == NULL)
        return;

    int other_id = message->other_id;
    message->other_id = message->client_id;
    message->client_id = other_id;

    send_message( message, client_queues[other_id] );
}

void handle_stop( msg_buffer * message ){
    if ( message->client_id < 0 || message->client_id >= MAX_CONNECTED_CLIENTS ) {
        return;
    }

    char content[MAX_MESSAGE_SIZE] = "Stopped";
    msg_buffer response = create_message(
        E_STOP, 
        content, 
        message->mq_name, 
        message->client_id, 
        -1
    );
    send_message(&response, client_queues[message->client_id] );
    
    mq_close(client_queues[message->client_id]);
    
    client_queues[message->client_id] = NULL;
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

void handle_message( msg_buffer * message ) {

    switch (message->command) {
        case E_INIT: handle_init(message); break;
        case E_LIST: handle_list(message); break;
        case E_2ALL: handle_2all(message); break;
        case E_2ONE: handle_2one(message); break;
        case E_STOP: handle_stop(message); break;
        default:     break;
    }

    printf("received: %s : %s\n", command_to_string(message->command), message->content);
    log_handled_message(message);
}


//// TERMINATION ////

void handle_exit() {
    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        if ( client_queues[id] == NULL)
            continue;

        char content[MAX_MESSAGE_SIZE] = "Server stopped";

        msg_buffer message = create_message(
            E_STOP, 
            content, 
            SERVER_QUEUE_NAME, 
            id, 
            -1
        );

        send_message(&message, client_queues[id]);
        mq_close(client_queues[id]);
    }

    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE_NAME);
        
    printf("server exit\n");
}

void handle_sigint(int pid) {
    printf("terminated by process : %d\n", pid);
    exit(0);
}

//// MAIN ////

int main () {

    //// SETUP TERMINATION ////

    setup_exit_handler(handle_sigint);
    atexit(handle_exit);

    //// CREATE MESSAGE QUEUE ////

    server_queue = create_queue(SERVER_QUEUE_NAME);

    //// SETUP CLIENT QUEUES ////

    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        client_queues[id] = NULL;
    }

    while (1) {
        msg_buffer message_buffer;
        
        if ( 0 <= mq_receive(
            server_queue, 
            (char *) &message_buffer, 
            MAX_MESSAGE_BUFFER_SIZE, 
            NULL
        )) {
            handle_message(&message_buffer);
        }

        sleep(1);
    }
    
    return 0;
}