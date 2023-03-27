#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#include "signals.h"

//// GETTERS AND SETTERS ////

int control_value = 0;

void set_control_value(int value) {
    control_value = value;
}
int  get_control_value() {
    return control_value;
}

//// HANDLERS ////

void handler_siginfo( 
    int signum, 
    siginfo_t *info, 
    void *ucontext 
) {
    printf("\n/////////////////////\n");
    
    printf("Signum:\t%d\n", signum);
    printf("Signal no:\t%d\n", info->si_signo);
    printf("Code:\t%d\n", info->si_code);
    printf("PID:\t%d\n", info->si_pid);
    printf("UID:\t%d\n", info->si_uid);
    printf("User time:\t%ld\n", info->si_utime);

    printf("/////////////////////\n");
    set_control_value(0);
    fflush(stdin);
}

void handler_resethand(int signum) {
    printf("Handle (%d)\n", signum);
}

int depth_id = 0;
int max_depth = 5;

void handler_nodefer(int signum) {
    int depth = depth_id++;
    char spaces[] = "               ";

    spaces[depth] = 0;

    printf("%sreceived[+]: %d | d:%d\n",spaces, signum, depth);
    fflush(stdout);
    if (depth < max_depth)
        raise(SIGUSR1);
    printf("%sreceived[-]: %d | d:%d\n",spaces, signum, depth);
    fflush(stdout);
}

//// TESTS ////


void test_siginfo() {
    printf("\n==[siginfo]=================\n");
    fflush(stdout);

    struct sigaction action;
    action.sa_sigaction = handler_siginfo;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO; 

    sigaction(SIGTSTP, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGUSR1, &action, NULL);

    /// this scenario is realized ///
    /// using raise/kill functions ///
    printf("\n[self]\n");

    set_control_value(1);
    while (get_control_value()) {
        sleep(1);
        raise(SIGUSR1);
    }

    /// this scenario is realized ///
    /// using forks and raise f. ///
    printf("\n[via fork]\n");

    set_control_value(1);
    if (fork() == 0) {
        kill(getppid(), SIGUSR1);
        return;
    } 
    
    while (get_control_value()) {
        sleep(1);
    }

    wait(NULL);
    /// this scenario is realized ///
    /// using user input/keyboard ///
    printf("\n[via CTRL+Z or CTRL+C]\n");

    set_control_value(1);
    while (get_control_value()) {
        sleep(1);
    }
}

void test_resethand() {
    printf("\n==[resethand]===============\n");
    fflush(stdout);


    struct sigaction action;
    action.sa_handler = handler_resethand;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESETHAND; 

    printf("[double press CTRL+C]\n");
    
    sigaction(SIGINT, &action, NULL);
    
    set_control_value(1);
    while (get_control_value()){
        sleep(1);
    }
        
}

void test_nodefer() {
    struct sigaction action;
    action.sa_handler = handler_nodefer;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_NODEFER; 

    printf("\n==[nodefer]=================\n");
    fflush(stdout);
    /// now we can use recursion in handler ///
    /// via signals ///

    sigaction(SIGUSR1, &action, NULL);
    raise(SIGUSR1); 
}

//// UTILS ////

void to_lower( char * str ) {
    for ( char * p = str; *p; p++ ) {
        *p = tolower(*p);
    }
}

//// ACTION TYPE ////

FlagType get_flag_type( const char * str ) {
    if (strcmp(str, "siginfo") == 0) {
        return Siginfo;
    }
    if (strcmp(str, "resethand") == 0) {
        return Resethand;
    }
    if (strcmp(str, "nodefer") == 0) {
        return Nodefer;
    }

    return None;
}
