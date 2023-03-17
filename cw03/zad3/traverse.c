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

int traverse( 
    const char * path,
    const char * pattern
) {
    printf("[path]: %s\n", path);
    
    DIR * dir           = NULL;
    struct dirent * dp  = NULL;

    dir = opendir(path);

    if (dir == NULL) {

        perror("unable to open directory");
        return -1;
    }

    while ( (dp = readdir(dir)) ) {

        if (strcmp(".", dp->d_name) == 0 || strcmp("..", dp->d_name) == 0)
            continue;

        char * filename = NULL;

        if ( (filename = malloc( PATH_MAX )) == NULL ) {
            perror("unable to allocate memory for path");
            return -1;
        }

        sprintf(filename,"%s/%s", path, dp->d_name);

        struct stat file_stats;

        if (stat(filename, &file_stats) == -1) {
            free(filename);
            continue;
        }

        if (!S_ISDIR(file_stats.st_mode)) {
            // check pattern
            
            free(filename);
            
            continue;
        }
        

        if ( fork() == 0 ) {

            int result = traverse(filename, pattern);

            free(filename);

            return result;
        }

    }

    while (wait(NULL) > 0);

    return 0;
}