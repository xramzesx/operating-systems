#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int main (int argc, char **argv) {

    //// VALIDATION ////

    if ( argc < 2 ) {
        printf("[error] pass process count as argument\n");
        return 1;
    }

    if ( argc > 2) {
        printf("[error] too many arguments\n");
        return 1;
    }

    for (int i = 0; i < strlen(argv[1]); i++) {
        if (!isdigit(argv[1][i])) {
            printf("[error] invalid argument, please pass integer\n");
            return 1;
        }
    }

    //// PARSING INPUT ////

    int n = atoi( argv[1] );

    //// SPAWNING FORKS ////

    for (int i = 0; i < n; i++) {
        if ( fork() == 0 ) {
            printf("[ppid]: %d\t[pid]: %d\n", getppid(), getpid());
            return 0;
        }
    }
    
    //// WAITING FOR ALL FORKS ////

    while(wait(NULL) > 0);
    
    //// FINISH ////

    printf("[n] = %d\n", n);
    
    return 0;
}