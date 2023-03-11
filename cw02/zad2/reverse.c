#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "reverse.h"

#define NS_SCALE 1000000000

//// UTILS ////

double get_time( struct timespec * ts_start, struct timespec * ts_end) {
    return (double)(ts_end->tv_sec - ts_start->tv_sec) + (double)(ts_end->tv_nsec - ts_start->tv_nsec) / NS_SCALE;
}

size_t min ( size_t a, size_t b ) {
    return a < b ? a : b;
}

char * rev_chunk (char *str, size_t chunk_size) {   
    char * result = malloc ( chunk_size + 1);
    
    result[chunk_size] = 0;

    for ( int i = 0; i < chunk_size; i++ ) {
        result[chunk_size - i - 1] = str[i];
    }

    return result;
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

    if ( !(source = fopen(f_source, "rb")) ) {
        printf("[error] unable to open file '%s'\n", f_source);
        return false;
    }

    if ( !(destination = fopen( f_destination, "wb")) ) {
        printf("[error] unable to open file '%s'\n", f_destination);
        fclose(source);
        return false;
    }

    //// REVERSE READ ////

    fseek( source, 0, SEEK_END );
    
    do {
        //// GET CHUNK SIZE ////
        size_t chunk_size = min(buffer_size, ftell(source));

        //// READ CHUNK /////

        fseek(source, -chunk_size, SEEK_CUR);
        fread(buffer, sizeof(char), chunk_size, source);
        fseek(source, -chunk_size , SEEK_CUR);

        //// WRITE CHUNK ////

        char * reversed = rev_chunk(buffer, chunk_size);

        fwrite( reversed, sizeof(char), chunk_size, destination);
        
        free(reversed);

    } while ( ftell(source) );


    //// CLOSE STREAMS ////

    fclose(source);
    fclose(destination);

    return true;
}