#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "replace.h"

int main (int argc, char **argv) {

    //// PREVALIDATION ////

    #if !defined(SYS) && !defined(LIB)

        printf("[error] compiled with fails - neither SYS nor LIB is defined\n");
        return 1;

    #endif

    //// VALIDATION ////

    if ( argc < 5 ) {
        printf("[error] not enough arguments\n");
        return 1;
    }

    if ( argc > 5 ) {
        printf("[error] too many arguments\n");
        return 1;
    }

    if (strlen(argv[1]) > 1) {
        printf("[error] invalid input: first parameter must be a char!\n");
        return 1;
    }

    if (strlen(argv[2]) > 1) {
        printf("[error] invalid input: second parameter must be a char!\n");
        return 1;
    }

    //// INITS ////

    char c_before = argv[1][0];
    char c_after  = argv[2][0];

    char * f_source      = argv[3];
    char * f_destination = argv[4];

    //// SYS ////

    #ifdef SYS

        //// PROCESSING ////

        struct timespec sys_real_start, sys_real_end;

        clock_gettime(CLOCK_REALTIME, &sys_real_start);
        if ( !sys_tr(c_before, c_after, f_source, f_destination) ) {
            printf("[error] process exit\n");
            return 1;
        }
        clock_gettime(CLOCK_REALTIME, &sys_real_end);

        //// RESULT ////
        
        printf("sys: %.6lf\n", get_time(&sys_real_start, &sys_real_end) );
    
    #endif

    //// LIB ////

    #ifdef LIB

        //// PROCESSING ////
        struct timespec lib_real_start, lib_real_end;

        clock_gettime(CLOCK_REALTIME, &lib_real_start);
        if( !lib_tr(c_before, c_after, f_source, f_destination) ) {
            printf("[error] process exit\n");
            return 1;
        }
        clock_gettime(CLOCK_REALTIME, &lib_real_end);

        //// RESULT ////

        printf("lib: %.6lf\n", get_time(&lib_real_start, &lib_real_end) );

    #endif

    return 0;
}