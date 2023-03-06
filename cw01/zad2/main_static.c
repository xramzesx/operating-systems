#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>

#include "lib.h"
//// REPL ///

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

//// MAIN ////

int main () {
    const char delimiters[] = " ";

    char main_buffer[MAX_LINE_LENGTH];
    char filename[MAX_BUFFER_SIZE];

    Data * data = NULL;
    size_t index = 0;


    while ( fgets(main_buffer, MAX_BUFFER_SIZE, stdin )) {

        char * buffer = main_buffer;

        while ( *buffer && isspace(*buffer) ) {
            buffer++;
        }

        //// TIME SETUP ////
        
        struct timespec ts_real_start, ts_real_end;
        struct tms tms_start, tms_end;

        clock_gettime(CLOCK_REALTIME, &ts_real_start);
        times(&tms_start);

        //// INIT REPL ////

        char argument[BUFSIZ];
        sscanf(buffer, "%s ", argument);
        to_lower(argument);

        //// REPL BODY /////

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

                if ( 0 != strcmp(strtok( buffer + strlen(argument), delimiters ) , "index" ) ) {
                    error_log("missing \"index\" argument; valid syntax: `delete index <index>`");
                    break;
                }

                if ( sscanf(strtok(NULL, delimiters), " %ld", &index ) != 1 ) {
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
        
            buffer = NULL;
        }

        //// TIME SUMMARIZE ////

        clock_gettime(CLOCK_REALTIME, &ts_real_end);
        times(&tms_end);


        time_log(
            ts_real_start, 
            ts_real_end, 
            tms_start, 
            tms_end
        );

    }
    


    return 0;
}