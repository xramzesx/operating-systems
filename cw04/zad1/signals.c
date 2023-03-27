#include <stdlib.h>
#include <stdio.h>

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "signals.h"

#include <string.h>
#include <ctype.h>

//// UTILS ////

void to_lower( char * str ) {
    for ( char * p = str; *p; p++ ) {
        *p = tolower(*p);
    }
}

//// ACTION TYPE ////

ActionType get_action_type( const char * str ) {

    if (strcmp(str, "ignore") == 0) {
        return Ignore;
    }
    if (strcmp(str, "handler") == 0) {
        return Handler;
    }
    if (strcmp(str, "mask") == 0) {
        return Mask;
    }
    if (strcmp(str, "pending") == 0) {
        return Pending;
    }

    return None;
}

//// SIGNALS ////

int child_pid = 0;

void signal_handler(int sig_no) {
    printf(
        "handle SIGUSR1 (%d) at %s\n", 
        sig_no, 
        child_pid == 0 
            ? "child" 
            : "parent"
    );
    
    fflush(stdout);
}

void show_pending_signals() {
    sigset_t pending;
    if (sigpending(&pending) < 0) {
        perror("sigpending");
        exit(1);
    }

    char *process_name = child_pid != 0 ? "parent" : "child"; 

    if (sigismember(&pending, SIGUSR1)) {
        printf("SIGUSR1 is pending in %s\n", process_name);
    } else {
        printf("SIGUSR1 is NOT pending in %s\n", process_name);
    }
    fflush(stdout);
}

void setup_signal(ActionType action) {
    struct sigaction s_action;
    s_action.sa_flags = 0;
    
    sigset_t mask;
    sigemptyset(&mask);
    sigemptyset(&s_action.sa_mask);

    switch (action) {
        case Ignore:
            printf("ignore\n");
            s_action.sa_handler = SIG_IGN;
            break;

        case Handler:
            printf("handler\n");
            s_action.sa_handler = signal_handler;

            break;

        case Mask:
        case Pending:

            printf( 
                action == Mask 
                    ? "mask\n" 
                    : "pending\n"
            );

            sigaddset(&mask, SIGUSR1);
            sigprocmask(SIG_SETMASK, &mask, NULL);

            break;

        case None:
        default:
            perror("Invalid argument type");
            exit(1);
            break;
    }

    sigaction(SIGUSR1, &s_action, NULL);
}

void set_child_pid(int ch_pid ) {
    child_pid = ch_pid;
}

int get_child_pid() {
    return child_pid;
}