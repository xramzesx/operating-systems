#pragma once
#include <stdbool.h>

//// UTILS ////

double get_time( struct timespec * ts_start, struct timespec * ts_end);

//// REPLACE FUNCTIONS ////

bool lib_tr ( 
    const char c_before, 
    const char c_after, 
    const char * f_source, 
    const char * f_destination 
);

bool sys_tr (
    const char c_before,
    const char c_after,
    const char * f_source,
    const char * f_destination
);