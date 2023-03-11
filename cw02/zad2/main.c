#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "reverse.h"

int main (int argc, char **argv) {

    //// VALIDATION ////

    if ( argc < 3 ) {
        printf("[error] not enough arguments\n");
        return 1;
    }

    if ( argc > 3 ) {
        printf("[error] too many arguments\n");
        return 1;
    }

    //// INITS ////

    char * f_source      = argv[1];
    char * f_destination = argv[2];

    //// PROCESSING ////

    struct timespec lib_real_start, lib_real_end;

    clock_gettime(CLOCK_REALTIME, &lib_real_start);
    if( !reverse_file(f_source, f_destination, 1024) ) {
        printf("[error] process exit\n");
        return 1;
    }
    clock_gettime(CLOCK_REALTIME, &lib_real_end);

    //// RESULT ////

    printf("buff[1]: %.6lf\n", get_time(&lib_real_start, &lib_real_end) );
    printf("buff[1024]: %.6lf\n", get_time(&lib_real_start, &lib_real_end) );


    return 0;
}