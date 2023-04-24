#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>

#include "shared.h"


int client_id = -1;
int client_msgid;

key_t server_key = -1;
int server_msgid = -1;

msg_buffer create_message(command cmd, char content[MAX_MESSAGE_SIZE]) {

    msg_buffer buffer;

    strcpy(buffer.content, content);
    buffer.client_id = client_id;
    buffer.command = cmd;
    buffer.client_msgid = client_msgid;

    switch (cmd) {
        case E_INIT:
        case E_STOP:;
            buffer.type = PRIORITY_STOP;
            break;
        
        case E_LIST:;
            buffer.type = PRIORITY_LIST;
            break;

        default:;
            buffer.type = PRIORITY_OTHER;
            break;
    }

    return buffer;
}

void send_message( msg_buffer * message ) {
    msgsnd(server_msgid, message, sizeof(*message), 0);
}

//// TERMINATION ////

void handle_exit() {
    msg_buffer message = create_message(E_STOP, "");
    send_message(&message);
    printf("client exit\n");
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

    server_key = ftok(PROJECT_PATHNAME, PROJECT_ID);
    server_msgid = msgget(server_key, 0666);

    msg_buffer message_buffer = create_message(E_INIT, "Witam bardzo serdecznie 8 D");

    send_message(&message_buffer);


    return 0;
}