#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define RANGE_START 0
#define RANGE_END   1

double f( double x ) {
    return 4 / (x * x + 1);
}

double integral( 
    double start, 
    double end, 
    double width
) {
    double result = 0;
    for (double x = start; x < end && x < RANGE_END; x += width) 
        result += f(x) * width;
    return result;
}

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

    int * pipes = calloc(forks_count, sizeof(int));

    for (int i = 0; i < forks_count; i++) {
        int fd[2];
        pipe(fd);
        
        int pid = fork();

        if (pid == 0) {
            close(fd[0]);

            double start = RANGE_START + (double) i / (double) forks_count;
            double end   = RANGE_START + (double) (i + 1) / (double) forks_count;

            double partial_result = integral(
                start,
                end,
                width
            );

            write(fd[1], &partial_result, sizeof(double));

            close(fd[1]);
            
            return 0;
        } 

        close(fd[1]);
        pipes[i] = fd[0];

        if (pid < 0) {
            perror("failed to fork");

            while (wait(NULL) > 0);
            
            for (int j = 0; j <= i; j++ ) {
                close(pipes[j]);
            }
            
            free(pipes);
            return 3;
        }
    }

    //// WAIT FOR ALL PROCCESSESS ///

    while (wait(NULL) > 0);
    
    //// PROCCED ///

    double result = 0;

    for (int i = 0; i < forks_count; i++) {
        double partial_result = 0;

        read(pipes[i], &partial_result, sizeof(double));
        close(pipes[i]);

        result += partial_result;
    }

    printf("Result: %lf\n", result);

    free(pipes);
    return 0;
}