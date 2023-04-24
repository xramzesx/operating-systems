#include <string.h>
#include <ctype.h>
#include <signal.h>

#include "shared.h"

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

void setup_exit_handler(sighandler_t handler) {
    struct sigaction sa;
    sa.sa_flags = 0;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;

    sigaction(SIGINT, &sa, NULL);
}
