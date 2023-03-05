#include <stdio.h>
#include "lib.h"


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


    



    return 0;
}