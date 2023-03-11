#pragma once
#include <sys/stat.h>

//// SETTERS AND GETTERS ////

void increase_total(long long bytes );
long long get_total();

//// PROCESS FUNCTION ////

int process_file (
    const char * path, 
    const struct stat * stats, 
    const int typeflag 
);