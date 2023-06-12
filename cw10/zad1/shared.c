#include <string.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <mqueue.h>
#include <unistd.h>

#include "shared.h"

//// MESSAGE ////

void set_content( 
    msg_buffer * message, 
    char content[MAX_MESSAGE_SIZE]
) {
    strcpy(message->content, content);
}

void set_mq_name( 
    msg_buffer * message, 
    char mq_name[MAX_QUEUE_NAME_SIZE]
) {
    strcpy(message->mq_name, mq_name);
}

msg_buffer create_message(
    command cmd, 
    char content[MAX_MESSAGE_SIZE],
    char mq_name[MAX_QUEUE_NAME_SIZE],
    int client_id,
    int other_id
) {

    msg_buffer buffer;

    set_content(&buffer, content);
    set_mq_name(&buffer, mq_name);

    buffer.client_id = client_id;
    buffer.command = cmd;
    buffer.client_id = client_id;
    buffer.other_id = other_id;

    time_t current_time = time(NULL);
    buffer.time = *localtime(&current_time);

    switch (cmd) {
        case E_INIT:
        case E_STOP:;
            buffer.mtype = PRIORITY_STOP;
            break;
        
        case E_LIST:;
            buffer.mtype = PRIORITY_LIST;
            break;

        default:;
            buffer.mtype = PRIORITY_OTHER;
            break;
    }

    return buffer;
}

mqd_t create_queue(const char * name) {
    struct mq_attr attributes;
    attributes.mq_maxmsg = MAX_CONNECTED_CLIENTS;
    attributes.mq_msgsize = MAX_MESSAGE_BUFFER_SIZE;
    return mq_open(name, O_RDWR | O_CREAT, 0666, &attributes);
}

void send_message( msg_buffer * message, int _socket ) {
    write(_socket, message, MAX_MESSAGE_BUFFER_SIZE);
}

//// UTILS ////

void to_upper_case(char * str) {
    for (char * ptr = str; *ptr; ptr++) {
        *ptr = toupper(*ptr);
    }
}

char * command_to_string(command cmd) {
    switch (cmd) {
        case E_INIT: return "INIT";
        case E_LIST: return "LIST";
        case E_2ALL: return "2ALL";
        case E_2ONE: return "2ONE";
        case E_STOP: return "STOP";
        case E_PING: return "PING";
        default:     return "None";
    }
}

command string_to_command(char * cmd) {
    to_upper_case(cmd);

    if ( strcmp(cmd, "INIT") == 0)
        return E_INIT;
    if ( strcmp(cmd, "LIST") == 0)
        return E_LIST;
    if ( strcmp(cmd, "2ALL") == 0)
        return E_2ALL;
    if ( strcmp(cmd, "2ONE") == 0)
        return E_2ONE;
    if ( strcmp(cmd, "STOP") == 0)
        return E_STOP;

    return E_NONE;        
}

//// TERMINATION ////

void setup_exit_handler(sighandler_t handler) {
    struct sigaction sa;
    sa.sa_flags = 0;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;

    sigaction(SIGINT, &sa, NULL);
}
