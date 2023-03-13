#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

long long traverse_dir (const char * path) {
    
    long long result = 0;

    DIR * dir           = NULL;
    struct dirent * dp = NULL;

    dir = opendir(path);

    if (dir == NULL) {
        printf("[error] unable to open '%s'\n", path);
        return -1;
    }

    while ( (dp = readdir( dir )) ) {
        struct stat file_stats;
        
        if ( stat(dp->d_name, &file_stats) == -1 )
            continue;

        if (S_ISDIR(file_stats.st_mode))
            continue;
        
        printf("%ld\t%s\n", file_stats.st_size ,dp->d_name);
        result += file_stats.st_size;
    }

    closedir(dir);

    printf("%lld\ttotal\n", result);

    return result;
}