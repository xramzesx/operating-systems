#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "reverse.h"

#define NS_SCALE 1000000000

//// UTILS ////

double get_time( struct timespec * ts_start, struct timespec * ts_end) {
    return (double)(ts_end->tv_sec - ts_start->tv_sec) + (double)(ts_end->tv_nsec - ts_start->tv_nsec) / NS_SCALE;
}

//// MAIN ////

bool reverse_file(
    const char * f_source,
    const char * f_destination,
    const size_t buffer_size
) {
    
    FILE * source      = NULL;
    FILE * destination = NULL;

    char buffer[buffer_size];

    //// OPEN FILE STREAMS ////

    if ( !(source = fopen(f_source, "r")) ) {
        printf("[error] unable to open file '%s'\n", f_source);
        return false;
    }

    if ( !(destination = fopen( f_destination, "w")) ) {
        printf("[error] unable to open file '%s'\n", f_destination);
        return false;
    }

    //// REVERSE READ ////

    fseek(source, 0, SEEK_END);
    
    size_t file_length = ftell(source);    

    while ( file_length ) {
        int block_length = file_length < buffer_size 
            ? file_length 
            : buffer_size;

        fseek(source, - block_length, SEEK_CUR);


        if ( block_length != fread(buffer, sizeof(char), block_length, source)) {
            printf("[error] failed to read file '%s'\n", f_source);
            return false;
        }

        
        for (int i = block_length - 1; i >= 0; i-- ){
            printf("%c", buffer[i]);
        }

        file_length -= block_length;
    }
    
    //// CLOSE STREAMS ////

    fclose(source);
    fclose(destination);

    return true;
}