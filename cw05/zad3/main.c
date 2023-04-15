#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <math.h>

#include "shared.h"

///// UTILS /////

double get_time( struct timespec * ts_start, struct timespec * ts_end) {
    return 
        (double)(ts_end->tv_sec - ts_start->tv_sec) + 
        (double)(ts_end->tv_nsec - ts_start->tv_nsec) / 
        NS_SCALE;
}

///// MAIN /////

int main (int argc, char ** argv) {

    ///// VALIDATION ////

    if (argc < 3) {
        perror("Too few arugments");
        return 1;
    }

    if ( argc > 3 ) {
        perror("Too many arguments");
        return 2;
    }

    //// PARSE ////

    double width    = atof(argv[1]);
    int forks_count = atoi(argv[2]);

    struct timespec lib_real_start, lib_real_end;

    clock_gettime(CLOCK_REALTIME, &lib_real_start);

    mkfifo(FIFO_PATH, 0666);
    int max_number_length = log10(forks_count) + 1;

    for (int i = 0; i < forks_count; i++) {
        int pid = fork();

        if (pid == 0) {
            char * str_index     = calloc(max_number_length, sizeof(char));
            char * str_processes = calloc(max_number_length, sizeof(char));
            char * str_width     = calloc(100, sizeof(char));

            sprintf(str_index, "%d", i);
            sprintf(str_processes, "%d", forks_count);
            sprintf(str_width, "%.10lf", width);

            execl(
                "./worker.exe", 
                "worker.exe", 
                str_index, 
                argv[2], 
                argv[1], 
                NULL
            );

            free(str_index);
            free(str_processes);
            free(str_width);
            return 0;
        } 

        if (pid < 0) {
            perror("failed to fork");

            return 3;
        }
    }

    //// PROCCED ///

    double result = 0;
    
    int fd = open(FIFO_PATH, O_RDONLY);

    for (int i = 0; i < forks_count; i++) {
        double partial_result = 0;
        size_t size = 0;
        while( (size = read(fd, &partial_result, sizeof(double))) != sizeof(double)) {
            if (size == -1) {
                perror("failed to read pipe");
                return 5;
            }
        };
        result += partial_result;
    }

    close(fd);
    clock_gettime(CLOCK_REALTIME, &lib_real_end);

    printf(
        "%.9lf\t%d\t%.6lf\t%lf\n", 
        width, 
        forks_count, 
        get_time(
            &lib_real_start, 
            &lib_real_end
        ), 
        result
    );

    remove(FIFO_PATH);
    return 0;
}