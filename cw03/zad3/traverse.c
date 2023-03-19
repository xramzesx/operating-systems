#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>

#include "traverse.h"

char * generate_error_message( 
    const char * error_message, 
    const char * path 
) {
    char * result = malloc( 
        strlen("[error] ") + strlen(error_message) + strlen(" ''") + strlen(path) + 1 
    );

    sprintf(result, "[error] %s '%s'", error_message, path);

    return result;
}

bool file_contains( 
    const char * filename, 
    const char * pattern 
) {

    FILE * file = NULL;

    char buffer[MAX_PATTERN_SIZE];

    //// OPEN FILE STREAMS ////

    if ( !(file = fopen(filename, "rb")) ) {
        char * error_message = generate_error_message(
            "unable to open directory", 
            filename
        );
        perror(error_message);
        free(error_message);
        return false;
    }

    fread(buffer, sizeof(char), MAX_PATTERN_SIZE, file);

    fclose(file);

    if (strlen(buffer) < strlen(pattern) ) {
        return false;
    }
    
    for (int i = 0; i < strlen(pattern); i++) {
        if (pattern[i] != buffer[i])
            return false;
    }

    return true;
}

int traverse( 
    const char * path,
    const char * pattern
) {    
    DIR * dir           = NULL;
    struct dirent * dp  = NULL;

    dir = opendir(path);

    if (dir == NULL) {
        char * error_message = generate_error_message(
            "unable to open file", 
            path
        );

        perror(error_message);
        
        free(error_message);
        
        return -1;
    }

    while ( (dp = readdir(dir)) ) {

        if (strcmp(".", dp->d_name) == 0 || strcmp("..", dp->d_name) == 0)
            continue;

        char * filename = NULL;

        if ( (filename = malloc( PATH_MAX )) == NULL ) {
            perror("unable to allocate memory for path");
            closedir(dir);
            return -1;
        }

        sprintf(filename,"%s/%s", path, dp->d_name);

        struct stat file_stats;

        if (stat(filename, &file_stats) == -1) {
            free(filename);
            continue;
        }

        if (S_ISREG(file_stats.st_mode)) {
            if (file_contains(filename, pattern))
                printf("%s\n", filename);

            free(filename);
            continue;
        }

        if (!S_ISDIR(file_stats.st_mode)) {
            free(filename);
            continue;
        }

        if ( fork() == 0 ) {

            int result = traverse(filename, pattern);

            free(filename);
            closedir(dir);

            return result;
        }

        free(filename);

    }

    closedir(dir);

    while (wait(NULL) > 0);

    return 0;
}