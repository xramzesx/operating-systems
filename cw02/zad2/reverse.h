#pragma once

#include <stdio.h>
#include <stdbool.h>

#include <unistd.h>
#include <time.h>
#include <sys/times.h>

//// UTILS ////

double get_time( struct timespec * ts_start, struct timespec * ts_end);

bool reverse_file(
    const char * f_source,
    const char * f_destination,
    const size_t buffer_size
);