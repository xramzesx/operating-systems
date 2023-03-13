#include <ftw.h>
#include <stdio.h>
#include <unistd.h>
#include "dir.h"

int main (int argc, char **argv) {

    if (argc < 2) {
        printf("[error] please pass the path\n");
        return 1;
    }

    if (argc > 2) {
        printf("[error] too many arguments\n");
        return 1;
    }

    const char * path = argv[1];

    if ( ftw( path, process_file, sysconf(_SC_OPEN_MAX) ) ) {
        printf("[error] traversing failed\n");
        return 1;
    }

    printf("%lld\ttotal\n", get_total());

    return 0;
}