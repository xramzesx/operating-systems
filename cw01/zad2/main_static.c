#include <stdio.h>
#include <string.h>
#include <time.h>
#include "lib.h"

#define MAX_BUFFER_SIZE 4096

enum Repl { 
    Init,    Count, 
    Show,    Delete, 
    Destroy, Default 
};

typedef enum Repl Repl;

Repl get_repl ( const char * command ) {
    if ( NULL == command ) 
        return Default;

    if ( 0 == strcmp(command, "init") )
        return Init;
    
    if ( 0 == strcmp(command, "count") )
        return Count;
    
    if ( 0 == strcmp(command, "show") )
        return Show;
    
    if ( 0 == strcmp(command, "delete") )
        return Delete;
    
    if ( 0 == strcmp(command, "destroy") )
        return Destroy;
    
    return Default;

}

int main () {

    char buffer[MAX_LINE_LENGTH];
    char filename[MAX_BUFFER_SIZE];

    Data * data = NULL;
    size_t index = 0;


    while ( fgets(buffer, MAX_BUFFER_SIZE, stdin )) {
        char * argument;
        sscanf(buffer, "%s", argument);

        char argument[BUFSIZ];
        sscanf(buffer, "%s ", argument);
        to_lower(argument);

        switch ( get_repl(argument) ) {
            case Init:
                size_t capacity = 0;

                if ( sscanf( buffer + strlen(argument), " %ld", &capacity ) != 1) {
                    error_log("invalid argument type");
                    break;
                }

                if ( NULL != data ) {
                    destroy_data(data);
                    data = NULL;
                    message_log("previous structure destroyed");
                }

                data = generate_data(capacity, 0);

                message_log("init success");
                break;
            
            case Count:
                
                if ( NULL == data ) {
                    error_log("uninitialized structure, init first!");
                    break;
                }

                
                filename[0] = 0;

                sscanf( buffer + strlen(argument), "%*[^\"]\"%[^\"]\"", filename );
                
                if ( strlen( filename ) == 0 && sscanf(buffer + strlen(argument), " %s", filename ) != 1 ) {
                    error_log("invalid argument type");
                    break;
                }
                
                if (word_count(data, filename))
                    message_log("words counted");
                else 
                    error_log("failed to count words");
                break;
            
            case Show:

                if ( NULL == data ) {
                    error_log("initialize first!");
                break;
                }
            
                index = 0;
            
                if ( sscanf( buffer + strlen(argument), " %ld", &index ) != 1 ) {
                    error_log("invalid argument type");
                break;
                };
            
                char * result = (char *) get_block(data, index); 
                if ( result )
                    printf("%s\n", result);
                else
                    printf("/empty/\n");
                result = NULL;
            
                break;
            
            case Delete:
                if ( NULL == data ) {
                    error_log("initialize first!");
                    break;
                }

                index = 0;

                if ( sscanf( buffer + strlen(argument), " %ld", &index ) != 1 ) {
                    error_log("invalid argument type");
                    break;
                };

                if (delete_block(data, index)) {
                    message_log("block deleted");
                } else {
                    message_log("block is already empty");
                }
            
                break;
            
            case Destroy:
            
                if ( NULL == data ) {
                    error_log("initialize first!");
                    break;
                }
                
                    destroy_data(data);
                    data = NULL;
                message_log("structure destroyed");
                break;
            
            default:
                error_log("unknown command");
                break;
        }

    }
    


    return 0;
}