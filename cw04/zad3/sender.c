#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int is_number(const char * str) {

    for (int i = 0; i < strlen(str); i++) {
        if ( !isdigit (str[i]) )
            return 0;
    }

    return 1;
}

void handler_sigusr1(
    int sig
) {
    //// PLACEHOLDER ////
}

void send_signal( 
    pid_t pid, 
    int signum, 
    int code
) {
    union sigval value;

    value.sival_int = code;

    sigset_t sigset;

    sigfillset(&sigset);
    sigdelset(&sigset, signum);
    sigdelset(&sigset, SIGINT);

    sigqueue(
        pid, 
        signum, 
        value
    );

    sigsuspend(&sigset);
    printf("received confirm: %d\n", code);
}

int main (int argc, char ** argv) {

    //// VALIDATION ////

    if (argc < 3) {
        perror("not enough arguments");
        return 1;
    }

    if ( !is_number(argv[1]) ) {
        perror("invalid pid");
        return 1;
    }

    pid_t pid = atoi(argv[1]);

    int n = argc - 2;
    int * commands = calloc(sizeof(int), argc - 2);

    for (int i = 2; i < argc; i++) {
        if (!is_number(argv[i])) {    
            free(commands);
            perror("invalid operating mode");
            return 1;
        }

        commands[i-2] = atoi(argv[i]);

        if (commands[i-2] > 5 || commands[i-2] < 0) {
            free(commands);
            perror("operating mode not found");
            return 1;
        }

        printf("%d\n", commands[i - 2]);

    }

    //// PREVENT DEFAULT SIGUSR1 ////
    
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = &handler_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);

    //// SENDING SIGNALS ////

    printf("[sender] pid: %d\n", getpid());

    for (int i = 0; i < n; i++ ){ 
        send_signal(pid, SIGUSR1, commands[i]);
    }

    return 0;
}