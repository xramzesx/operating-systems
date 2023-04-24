#include <stdlib.h>

//// GENERAL ////

#define MAX_MESSAGE_SIZE      512
#define MAX_CONNECTED_CLIENTS 5

//// FTOK ////

#define PROJECT_PATHNAME getenv("HOME")
#define PROJECT_ID       1337

//// COMMAND PRIORITIES ////

#define PRIORITY_STOP   1
#define PRIORITY_LIST   2
#define PRIORITY_OTHER  3

//// COMMANDS ////

typedef enum {
    E_INIT, E_LIST, 
    E_2ALL, E_2ONE, 
    E_STOP, E_NONE
} command;

typedef void (*sighandler_t)(int);

//// MSGP STRUCTURE ////

typedef struct {
    long type;
    char content[MAX_MESSAGE_SIZE];
    command command;
    key_t client_msgid;
    int client_id;
} msg_buffer;

//// UTILS ////

char * command_to_string(command cmd);
command string_to_command(char * cmd);

//// TERMINATION ////

void setup_exit_handler(sighandler_t handler);