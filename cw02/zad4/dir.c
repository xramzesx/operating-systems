#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#include "dir.h"

long long total_size = 0;

void increase_total(long long bytes ) {
    total_size += bytes;
}

long long get_total() {
    return total_size;
}

int process_file (
    const char * path, 
    const struct stat * stats, 
    const int typeflag 
) {
    if (S_ISDIR(stats->st_mode))
        return 0;

    printf("%ld\t%s\n", stats->st_size, path);

    increase_total(stats->st_size);

    return 0;
}