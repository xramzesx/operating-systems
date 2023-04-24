#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>

#include "shared.h"

//// CACHE ////

key_t server_key = -1;
int server_msgid = -1;

int client_msgids[MAX_CONNECTED_CLIENTS];

int is_connected_client(msg_buffer * message ){
    return 
        message->client_id >= 0 && 
        message->client_id < MAX_CONNECTED_CLIENTS &&
        client_msgids[message->client_id] != -1;
}

int check_connection(msg_buffer * message) {
    if (is_connected_client(message))
        return 1;

    msg_buffer response = create_message(
        message->command, 
        "Connection failed: client id not found", 
        server_msgid, 
        message->client_id, 
        -1
    );
    send_message(&response, message->client_msgid);

    return 0;
}

//// HANDLERS ////

void handle_init( msg_buffer * message ) {
    int client_id = -1;

    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        if (client_msgids[id] == -1) {
            client_id = id;
            client_msgids[client_id] = message->client_msgid;
            break;
        }
    }

    if (client_id == -1) {
        msg_buffer response = create_message(
            E_INIT, 
            "Connection failed: too many connected clients", 
            server_msgid, 
            client_id, 
            -1
        );
        send_message(&response, message->client_msgid);
        return;
    }

    msg_buffer response = create_message(
        E_INIT, 
        "Connection success", 
        server_msgid, 
        client_id, 
        -1
    );

    send_message(&response, message->client_msgid);
}

void handle_list( msg_buffer * message ){
    if (!check_connection(message)) return;

    char buffer[MAX_MESSAGE_SIZE];

    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        if (client_msgids[id] != -1) {
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
        client_msgids[message->client_id], 
        message->client_id, 
        -1
    );

    send_message(&response, client_msgids[message->client_id]);
}

void handle_2all( msg_buffer * message ){
    if (!check_connection(message)) return;
    
    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++) {
        if (client_msgids[id] != -1 && id != message->client_id) {
            printf("Send to %d %d\n", id, client_msgids[id]);
            msg_buffer tmp_msg = create_message(E_2ALL, message->content, client_msgids[id], id, message->client_id);
            send_message(&tmp_msg, client_msgids[id]);
        }
    }
}

void handle_2one( msg_buffer * message ){
    if (!check_connection(message)) return;

    if (message->other_id < 0 || message->other_id >= MAX_BUFFER_SIZE)
        return;

    if (client_msgids[message->other_id] == -1)
        return;

    send_message(message, client_msgids[message->other_id]);
}

void handle_stop( msg_buffer * message ){
    if ( message->client_id < 0 || message->client_id >= MAX_CONNECTED_CLIENTS ) {
        return;
    }

    client_msgids[message->client_id] = -1;
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
        if ( client_msgids[id] == -1)
            continue;

        msg_buffer message = create_message(E_STOP, "Server stopped", client_msgids[id], id, -1);
        send_message(&message, client_msgids[id]);
    }
    msgctl(server_msgid, IPC_RMID, NULL);
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

    server_key   = ftok(PROJECT_PATHNAME, PROJECT_ID);
    server_msgid = msgget(server_key, 0666 | IPC_CREAT);

    for (int id = 0; id < MAX_CONNECTED_CLIENTS; id++)
        client_msgids[id] = -1;

    while (1) {
        msg_buffer message_buffer;
        
        if ( 0 <= msgrcv(
            server_msgid, 
            &message_buffer, 
            sizeof(msg_buffer), 
            PRIORITY_MSGTYP, 
            0 
        )) {
            handle_message(&message_buffer);
        }
        sleep(1);
    }
    
    return 0;
}