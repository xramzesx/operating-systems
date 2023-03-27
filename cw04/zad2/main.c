#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "signals.h"

int main(int argc, char ** argv ) {

    if ( argc != 2 ) {
        perror ( "wrong number of arguments" );
        return 1;
    }

    char * ptr = argv[1];

    to_lower(ptr);

    FlagType flag = get_flag_type(ptr);

    switch (flag){
        case Siginfo:
            test_siginfo();
            break;
        case Resethand:
            test_resethand();
            break;

        case Nodefer:
            test_nodefer();
            break;
        
        default:
            break;
    }

    return 0;
}