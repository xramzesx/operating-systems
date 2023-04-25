#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>

#include "shared.h"

int client_id = -1;

mqd_t server_queue;
mqd_t client_queue;

char client_queue_name[MAX_QUEUE_NAME_SIZE];
int  server_closed = 0;

msg_buffer c_create_message(command cmd, char content[MAX_MESSAGE_SIZE]) {
    return create_message(cmd, content, client_queue_name, client_id, -1);
}

void c_send_message( msg_buffer * message ) {
    send_message( message, server_queue );
}

void handle_server_response() {
    msg_buffer * message = calloc(1, sizeof(msg_buffer));

    mq_receive(client_queue, (char *) message, MAX_MESSAGE_BUFFER_SIZE, NULL);
    
    switch (message->command) {
        case E_2ONE:
            printf(
                "[%02d:%02d:%02d] ", 
                message->time.tm_hour, 
                message->time.tm_min, 
                message->time.tm_sec
            );
        case E_2ALL:
            printf("from: %d:\n", message->other_id);
        case E_LIST:
            printf("%s\n", message->content);
            break;
        case E_STOP:
            free(message);
            server_closed = 1;
            exit(0);
            break;
        default:
            break;
    }

    free(message);
}

void check_server_response() {
    struct mq_attr attributes;
    
    mq_getattr( client_queue, &attributes );

    while (attributes.mq_curmsgs > 0) {
        handle_server_response();
        mq_getattr( client_queue, &attributes );
    }
}

//// TERMINATION ////

void handle_exit() {
    msg_buffer message = c_create_message(E_STOP, "stop");
    if (server_closed == 0) {
        c_send_message(&message);
        mq_receive(client_queue, (char *) &message, MAX_MESSAGE_BUFFER_SIZE, NULL);
    }

    mq_close(client_queue);
    mq_unlink(client_queue_name);

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


    sprintf(client_queue_name, "/posix-client-%d", getpid());

    server_queue = mq_open(SERVER_QUEUE_NAME, O_RDWR);
    client_queue = create_queue(client_queue_name);


    //// SEND INIT COMMAND ////

    msg_buffer message_buffer = c_create_message(E_INIT, client_queue_name);
    c_send_message(&message_buffer);

    mq_receive(client_queue, (char *) &message_buffer, MAX_MESSAGE_BUFFER_SIZE, NULL);

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
                handle_server_response();
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