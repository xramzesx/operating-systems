#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "traverse.h"

int main (int argc, char **argv) {

    //// VALIDATION ////

    if ( argc < 3 ) {
        printf("[error] too few arguments\n");
        return 1;
    }

    if ( argc > 3) {
        printf("[error] too many arguments\n");
        return 1;
    }

    if ( strlen(argv[2]) > MAX_PATTERN_SIZE ) {
        printf("[error] too long pattern\n");
        return 1;
    }

    //// MAIN ////

    const char * path    = argv[1];
    const char * pattern = argv[2];

    traverse(path, pattern);
    
    return 0;
}