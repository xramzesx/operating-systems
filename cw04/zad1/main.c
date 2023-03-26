#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "signals.h"

//// MAIN ////

int main (int argc, char **argv) {
    set_child_pid( getpid() );

    if (argc < 2) {
        perror("not enough arguments");
        return 1;
    }

    if (argc > 3) {
        perror("too many arguments");
        return 1;
    }

    //// PARSE ////

    to_lower(argv[1]);
    ActionType action = get_action_type(argv[1]);

    //// INITIALIZE SIGNAL SUPPORT ////

    int is_in_child = argc == 3 && strcmp("child", argv[2]) == 0;

    if (!is_in_child) {
        setup_signal(action);
        raise(SIGUSR1);

        if ( action == Mask || action == Pending ) {
            show_pending_signals();
        }
    }


    set_child_pid(fork());

    if (get_child_pid() == 0) {

    if ( action == Mask || action == Pending ) {
        show_pending_signals();
    }

    if ((child_pid = fork()) == 0) {
        if ( action != Pending ) {
            raise(SIGUSR1);
        } 

        if (action == Pending || action == Mask)
            show_pending_signals();
    }

    wait(NULL);
    return 0;
}