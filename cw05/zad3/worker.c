#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "shared.h"

//// FUNCTION & INTEGRATION ////

double f( double x ) {
    return 4 / (x * x + 1);
}

double integral( 
    double start, 
    double end, 
    double width
) {
    double result = 0;
    for (double x = start; x < end && x < RANGE_END; x += width) {
        result += f(x) * width;
    }
    return result;
}

///// MAIN /////

int main (int argc, char ** argv) {

    ///// VALIDATION ////

    if (argc < 4) {
        perror("Too few arugments");
        return 1;
    }

    if ( argc > 4 ) {
        perror("Too many arguments");
        return 2;
    }

    //// PARSE ////

    int index = atoi(argv[1]);
    int processes = atoi(argv[2]);

    double width  = atof(argv[3]);

    //// INTEGRATE ////
    
    double start = RANGE_START + (double) index / (double) processes;
    double end   = RANGE_START + (double) (index + 1) / (double) processes;

    double partial_result = integral(
        start,
        end,
        width
    );

    //// SEND RESULT BACK ////

    int fd = open(FIFO_PATH, O_WRONLY);

    if ( write(fd, &partial_result, sizeof(double)) == -1 ) {
        perror("failed to write to fifo");
        return 3;
    }

    close(fd);
    return 0;
}