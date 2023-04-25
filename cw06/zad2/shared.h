#include <stdlib.h>
#include <time.h>

//// GENERAL ////

#define MAX_MESSAGE_SIZE      512
#define MAX_CONNECTED_CLIENTS 5
#define MAX_BUFFER_SIZE       (2 * MAX_MESSAGE_SIZE)
//// FTOK ////

#define PROJECT_PATHNAME getenv("HOME")
#define PROJECT_ID       1337

//// COMMON CONSTANTS ////

#define MAX_PROJECT_ID   (PROJECT_ID * PROJECT_ID)
#define DELIMITERS       " \t\n"

//// COMMAND PRIORITIES ////

#define PRIORITY_MSGTYP -10

#define PRIORITY_STOP   3
#define PRIORITY_LIST   2
#define PRIORITY_OTHER  1

//// COMMANDS ////

typedef enum {
    E_INIT, E_LIST, 
    E_2ALL, E_2ONE, 
    E_STOP, E_NONE
} command;

typedef void (*sighandler_t)(int);

//// MSGP STRUCTURE ////

typedef struct {
    long mtype;
    char content[MAX_MESSAGE_SIZE];
    command command;
    key_t client_msgid;
    int client_id;
    int other_id;
    struct tm time;
} msg_buffer;


//// MESSAGE ////

void set_content( 
    msg_buffer * message, 
    char content[MAX_MESSAGE_SIZE]
);

msg_buffer create_message(
    command cmd, 
    char content[MAX_MESSAGE_SIZE],
    int client_msgid,
    int client_id,
    int other_id
);

void send_message( 
    msg_buffer * message, 
    int msgid 
);

//// UTILS ////

char * command_to_string(command cmd);
command string_to_command(char * cmd);

//// TERMINATION ////

void setup_exit_handler(sighandler_t handler);