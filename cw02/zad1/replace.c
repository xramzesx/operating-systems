#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "replace.h"

#define NS_SCALE 1000000000

//// UTILS ////

double get_time( struct timespec * ts_start, struct timespec * ts_end) {
    return (double)(ts_end->tv_sec - ts_start->tv_sec) + (double)(ts_end->tv_nsec - ts_start->tv_nsec) / NS_SCALE;
}

//// REPLACE FUNCTIONS ////

bool lib_tr ( 
    const char c_before, 
    const char c_after, 
    const char * f_source, 
    const char * f_destination 
) {
    FILE * source      = NULL;
    FILE * destination = NULL;

    //// OPEN FILE STREAMS ////

    if ( !(source = fopen(f_source, "r")) ) {
        printf("[error] unable to open file '%s'", f_source);
        return false;
    }

    if ( !(destination = fopen( f_destination, "w")) ) {
        printf("[error] unable to open file '%s'", f_destination);
        return false;
    }

    //// FILE PROCESSING ////

    fseek( source, SEEK_SET, 0 );

    char c_buffer;

    while ( fread(&c_buffer, sizeof(c_buffer), 1, source) ) {
        fwrite(
            c_before == c_buffer 
                ? &c_after 
                : &c_buffer, 
            sizeof(c_buffer), 
            1, 
            destination
        );
    }

    //// CLOSE STREAMS ////

    fclose(source);
    fclose(destination);

    return true;
}


bool sys_tr (
    const char c_before,
    const char c_after,
    const char * f_source,
    const char * f_destination
) {
    //// OPEN STREAMS ////

    int i_source = open(f_source, O_RDONLY);
    int i_destination = open(f_destination, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    char c_buff;

    //// FILES VALIDATION ////

    if ( -1 == i_source ) {
        printf("[error]: unable to find file '%s'\n", f_source);
        return false;
    }

    if ( -1 == i_destination ) {
        printf("[error]: unable to write to file '%s'\n", f_destination);
        return false;
    }

    //// FILE PROCESSING ////

    while (read( i_source, &c_buff, 1)) {
        write(
            i_destination, 
            c_buff == c_before 
                ? &c_after 
                : &c_buff, 
            1
        );
    }

    return true;

}