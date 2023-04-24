#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>

#include "shared.h"

int client_id = -1;

key_t server_key = -1;
key_t client_key = -1;

int server_msgid = -1;
int client_msgid = -1;


msg_buffer c_create_message(command cmd, char content[MAX_MESSAGE_SIZE]) {
    return create_message(cmd, content, client_msgid, client_id, -1);
}

void c_send_message( msg_buffer * message ) {
    send_message( message, server_msgid );
}

void handle_server_response() {
    msg_buffer * message = calloc(2, sizeof(msg_buffer));
    // printf("sssss");

    msgrcv(client_msgid, message, sizeof(*message), PRIORITY_MSGTYP, 0);

    switch (message->command) {
        case E_2ONE:
            printf(
                "[%02d:%02d:%02d] ", 
                message->time.tm_hour, 
                message->time.tm_min, 
                message->time.tm_sec
            );
        case E_2ALL:
            printf("from: %d:\n", message->client_id);
        case E_LIST:
            printf("%s\n", message->content);
            break;
        case E_STOP:
            free(message);
            exit(0);
            break;
        default:
            break;
    }

    free(message);
}

void check_server_response() {
    struct msqid_ds msqid_stat;
    msgctl(client_msgid, IPC_STAT,&msqid_stat);

    printf("remaining messages: %ld\n", msqid_stat.msg_qnum);

    while (msqid_stat.msg_qnum > 0) {
        handle_server_response();
        msgctl(client_msgid, IPC_STAT,&msqid_stat);
    }
}

//// TERMINATION ////

void handle_exit() {
    msg_buffer message = c_create_message(E_STOP, "stop");
    c_send_message(&message);
    
    msgctl(client_msgid, IPC_RMID, NULL);
    printf("client exit\n");
}

void handle_sigint(int pid) {
    printf("terminated by process : %d\n", pid);
    exit(0);
}

//// MAIN ////

int main () {
    srand(time(NULL));

    //// SETUP TERMINATION ////

    setup_exit_handler(handle_sigint);
    atexit(handle_exit);

    //// SETUP MESSAGE QUEUE ////

    server_key = ftok(PROJECT_PATHNAME, PROJECT_ID);
    server_msgid = msgget(server_key, 0666);

    client_key = ftok(PROJECT_PATHNAME, rand() % PROJECT_ID + 1);
    client_msgid = msgget(client_key, 0666 | IPC_CREAT);

    printf("[%d, %d]\n", server_msgid, client_msgid);

    if (server_msgid == -1) {
        printf("This server is turned off\n");
        return 0;
    }

    //// SEND INIT COMMAND ////

    msg_buffer message_buffer = c_create_message(E_INIT, "init");
    c_send_message(&message_buffer);

    msgrcv( client_msgid, &message_buffer, sizeof(message_buffer), 0, 0 );
    client_id = message_buffer.client_id;

    if (client_id == -1) {
        printf("This server is busy\n");
        return 0;
    }

    //// REPL ////

    int repl_active = 1;
    
    while (repl_active) {
        check_server_response();

        char * buffer = NULL;
        size_t buff_len;

        int readed = getline(&buffer, &buff_len, stdin);
        buffer[readed - 1] = 0;

        char * ptr = strtok(buffer, DELIMITERS);

        if (ptr == NULL)
            continue;

        char *  cmd_str = ptr;
        command cmd = string_to_command(cmd_str);

        msg_buffer message = c_create_message(cmd, "tmp");

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
                
                set_content(&message, ptr );

                c_send_message(&message);
                break;

            case E_LIST:
                c_send_message(&message);

                handle_server_response();
                break;

            case E_STOP:
                // c_send_message(&message);
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