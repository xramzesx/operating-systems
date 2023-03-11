#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "replace.h"

#define NS_SCALE 1000000000

int main (int argc, char **argv) {

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

    for (int i = 0; i < argc; i++ )
        printf("%s\n", argv[i]);

    //// SYS ////

    struct timespec sys_real_start, sys_real_end;

    clock_gettime(CLOCK_REALTIME, &sys_real_start);
    sys_tr(c_before, c_after, f_source, f_destination);
    clock_gettime(CLOCK_REALTIME, &sys_real_end);


    //// LIB ////

    struct timespec lib_real_start, lib_real_end;

    clock_gettime(CLOCK_REALTIME, &lib_real_start);
    lib_tr(c_before, c_after, f_source, f_destination);
    clock_gettime(CLOCK_REALTIME, &lib_real_end);
    

    //// RESULTS ////

    printf("sys: %.6lf\n", get_time(&sys_real_start, &sys_real_end) );
    printf("lib: %.6lf\n", get_time(&lib_real_start, &lib_real_end) );

    return 0;
}