#include <stdio.h>
#include "lib.h"

#define BUFFER_SIZE 4096

enum Repl { 
    Init,    Count, 
    Show,    Delete, 
    Destroy, Default 
};

typedef enum Repl Repl;

Repl get_repl ( const char * command ) {

    if ( 0 == strcmp(command, "init") )
        return Init;
    
    if ( 0 == strcmp(command, "count") )
        return Count;
    
    if ( 0 == strcmp(command, "delete") )
        return Delete;
    
    if ( 0 == strcmp(command, "destroy") )
        return Destroy;
    
    return Default;

}

int main () {

    char buffer[MAX_LINE_LENGTH];
    Data * data = NULL;


    while ( fgets(buffer, BUFFER_SIZE, stdin )) {
        char * argument;
        sscanf(buffer, "%s", argument);

        to_lower(argument);

        switch ( get_repl(argument) ) {
            case Init:
                if ( NULL != data ) {
                    destroy_data(data);
                    data = NULL;
                }

                size_t capacity = 0;

                if ( sscanf( buffer + strlen(argument), "%ld", &capacity )) {
                    error_log("ivalid type for argument");
                    break;
                }

                data = generate_data(capacity, 0);

                message_log("init success");
                break;
            
            case Count:
            
                break;
            
            case Show:
            
                break;
            
            case Delete:
            
                break;
            
            case Destroy:
                    destroy_data(data);
                    data = NULL;
                break;
            
            default:
                break;
        }

    }
    


    return 0;
}