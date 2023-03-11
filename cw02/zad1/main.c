#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#define NS_SCALE 1000000000

void lib_tr ( 
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
        return;
    }

    if ( !(destination = fopen( f_destination, "w")) ) {
        printf("[error] unable to open file '%s'", f_destination);
        return;
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

}


void sys_tr (
    const char c_before,
    const char c_after,
    const char * f_source,
    const char * f_destination
) {

    //// OPEN STREAMS ////

    int i_source = open(f_source, O_RDONLY);
    int i_destination = open(f_destination, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    char c_buff;

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

}

double get_time( struct timespec * ts_start, struct timespec * ts_end) {
    return (double)(ts_end->tv_sec - ts_start->tv_sec) + (double)(ts_end->tv_nsec - ts_start->tv_nsec) / NS_SCALE;
}

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