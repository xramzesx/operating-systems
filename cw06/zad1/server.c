#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "shared.h"

//// CACHE ////

int client_keys[MAX_CONNECTED_CLIENTS];

//// HANDLERS ////

void handle_init( msg_buffer * message ) {

}

void handle_list( msg_buffer * message ){
    
}

void handle_2all( msg_buffer * message ){
    
}

void handle_2one( msg_buffer * message ){
    
}

void handle_stop( msg_buffer * message ){
    
}

void handle_message( msg_buffer * message ) {

    printf("received: %s : %s\n", command_to_string(message->command), message->content);
}

//// MAIN ////

int main () {
    
    //// CREATE MESSAGE QUEUE ////

    key_t key = ftok(PROJECT_PATHNAME, PROJECT_ID);
    int main_msgid = msgget(key, 0666 | IPC_CREAT);

    while (1) {
        msg_buffer message_buffer;
        
        msgrcv(
            main_msgid, 
            &message_buffer, 
            sizeof(msg_buffer), 
            0, 
            0 
        );


        handle_message(&message_buffer);

    }
    
	
    msgctl(main_msgid, IPC_RMID, NULL);

    return 0;
}