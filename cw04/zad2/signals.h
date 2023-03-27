#pragma once
#include <signal.h>

//// HANDLERS ////

void handler_siginfo( 
    int signum, 
    siginfo_t *info, 
    void *ucontext 
);
void handler_resethand(int signum);
void handler_nodefer(int signum);

//// TESTS ////

void test_siginfo();
void test_resethand();
void test_nodefer();

//// ENUMS ////

enum FlagType {
    Siginfo, Resethand, Nodefer, None
};

typedef enum FlagType FlagType;

//// UTILS ////

void to_lower( char * str );

//// ACTION TYPE ////

FlagType get_flag_type( const char * str );

//// GETTERS AND SETTERS ////

void set_control_value(int value);
int  get_control_value();