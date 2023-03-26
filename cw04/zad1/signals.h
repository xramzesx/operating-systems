#pragma once

void to_lower( char * str );

enum ActionType {
    Ignore, Handler, Mask, Pending, None
};

typedef enum ActionType ActionType;

ActionType get_action_type( const char * str );

void setup_signal(ActionType action);
void signal_handler(int sig_no);
void show_pending_signals();

void set_child_pid(int ch_pid );
int get_child_pid();