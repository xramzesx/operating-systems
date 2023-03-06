#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>

#include "lib.h"

//// DYNAMIC ONLY ////

#ifdef DLL
    #include <dlfcn.h>
#endif

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

    //// DYNAMIC ONLY ////

    #ifdef DLL
        void *hook = dlopen("./liblib.so", RTLD_LAZY );
        if (!hook) {
            printf("[error]: unable to open library `liblib.so`\n");
            return 1;
        }

        void (*error_log)(const char *) = dlsym( hook, "error_log");

        if (dlerror() != NULL) {
            printf("[error]: unable to hook `error_log` function\n");
            return 1;
        }

        void (*message_log)(const char *) = dlsym( hook, "message_log");

        if (dlerror() != NULL) {
            error_log("unable to hook `message_log` function");
            return 1;
        }

        void (*time_log)( 
            struct timespec,
            struct timespec,
            struct tms,
            struct tms 
        ) = dlsym(hook, "time_log");

        if (dlerror() != NULL) {
            error_log("unable to hook `time_log` function");
            return 1;
        }

        void (*to_lower)(char * str) = dlsym(hook, "to_lower");

        if (dlerror() != NULL) {
            error_log("unable to hook `to_lower` function");
            return 1;
        }

        Data * (*generate_data) (size_t, size_t) = dlsym(hook, "generate_data");
        
        if (dlerror() != NULL) {
            error_log("unable to hook `generate_data` function");
            return 1;
        }

        void (*destroy_data) ( Data *) = dlsym(hook, "destroy_data");
        
        if (dlerror() != NULL) {
            error_log("unable to hook `destroy_data` function");
            return 1;
        }

        int (*delete_block) ( Data *, size_t ) = dlsym(hook, "delete_block");
        
        if (dlerror() != NULL) {
            error_log("unable to hook `delete_block` function");
            return 1;
        }

        void * (*get_block) ( Data *, size_t ) = dlsym(hook, "get_block");
        
        if (dlerror() != NULL) {
            error_log("unable to hook `get_block` function");
            return 1;
        }

        int (* word_count) ( Data *data, const char *filename ) = dlsym(hook, "word_count");
        
        if (dlerror() != NULL) {
            error_log("unable to hook `word_count` function");
            return 1;
        }
    #endif
    
    const char delimiters[] = " ";
    
    char main_buffer[MAX_BUFFER_SIZE];
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
    

    //// END ////

    #ifdef DLL
        dlclose(hook);
    #endif

    return 0;
}