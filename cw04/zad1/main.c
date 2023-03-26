#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#include <string.h>
#include <ctype.h>

void to_lower( char * str ) {
    for ( char * p = str; *p; p++ ) {
        *p = tolower(*p);
    }
}

enum ActionType {
    Ignore, Handler, Mask, Pending, None
};

typedef enum ActionType ActionType;

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

int child_pid = 0;

void signal_handler(int sig_no) {
    printf(
        "SIGUSR1 (%d) at %s\n", 
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

//// MAIN ////

int main (int argc, char **argv) {
    child_pid = getpid();

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
            return 1;
            break;
    }


    sigaction(SIGUSR1, &s_action, NULL);

    raise(SIGUSR1);

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

    return 0;
}