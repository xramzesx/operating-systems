#include <stdio.h>
#include <unistd.h>

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

    //// MAIN ////

    printf("[%s] ", argv[0]);
    fflush(stdout);
    execl("/bin/ls", "ls" , argv[1], NULL);

    return 0;
}