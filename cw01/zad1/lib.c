#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lib.h"

//// UTILS ////

void message_log(const char * message) {
    printf("\033[0;32m[log]\033[0m: %s\n", message);
}

void error_log(const char * error_message) {
    printf("\033[0;31m[error]\033[0m: %s\n", error_message);
}

void to_lower(char * str) {
    for (char *p = str; *p; p++)
        *p = tolower(*p);
}


//// STRUCTIRE METHODS ///

Data * generate_data (size_t capacity, size_t size) {

    if (capacity < 0) {
        capacity = 0;
        size = 0;
    }

    if (size < 0) {
        size = 0;
    }

    Data *data = malloc(sizeof(Data));

    data->blocks = calloc(capacity, sizeof(void*));
    data->capacity = capacity;
    data->size = size;

    return data;
}

void add_block (Data *data, void * block) {
    if (data->capacity <= data->size )
        return;
    
    data->blocks[data->size++] = block;
}

void delete_block (Data * data, size_t index) {
    if ( data->capacity <= index )
        return;
    
    if ( index < 0 )
        return;
    
    if ( data->blocks[index] != NULL ) {
        free(data->blocks[index]);
        data->blocks[index] = NULL;
    }
    
}

void * get_block(Data * data, size_t index) {
    if ( data->capacity <= index ) {
        return NULL;
    }

    return data->blocks[index];
}

void destroy_data (Data * data) {
    for ( size_t i = 0; i < data->capacity; i++ ) {
        delete_block(data, i);
    }
    
    /// note: remember to null data after this function!
    free(data);
}

//// FILE UTILS ////

char * read_file_block(const char *filename) {

    FILE * file = fopen(filename, "a+");
    char line[MAX_BLOCK_SIZE];

    if (NULL == file) {
        error_log("unable to open file");
        exit(1);
    }

    /// note: block files are single-line 
    if (NULL == fgets(line, MAX_BLOCK_SIZE, file)) {
        error_log("corrupted file");
        exit(1);
    }

    //// TRIM ////

    line[strcspn(line, "\r\n")] = 0;
    char * ptr = line;

    while ( *ptr != 0 && isspace(*ptr) ) {
        ptr++;
    }

    //// ALLOCATE MEMORY TO BLOCK /////

    void * block = calloc( strlen(ptr) + 1, sizeof(char) );
    strcpy(block, ptr);

    fclose(file);
    ptr = NULL;

    return block;
}

//// MAIN METHOD ////

void word_count (Data *data, const char *filename) {
    //// VALIDATE SIZE ////

    if (data->capacity == data->size ) {
        error_log("structure is full");
        return;
    }
    
    //// GENERATE TMP FILENAME ////
    
    char tmp_filename[] = "/tmp/jk-wc-XXXXXX";
    
    if ( mkstemp(tmp_filename) == -1 ) {
        error_log("failed to generate temporary filename");
        exit(1);
    }

    //// GENEREATE TMP FILE ////

    char *command = calloc(
        strlen("wc \"") + 
        strlen(filename) + 
        strlen("\" > ") + 
        strlen(tmp_filename) + 
        1,
        sizeof(char)
    );
    

    if ( command == NULL ) {
        error_log("failed to allocate memory for command `wc`");
        exit(1);
    }

    if (sprintf( command, "wc \"%s\" > %s", filename, tmp_filename ) == -1) {
        error_log("failed to concatenate string");
        exit(1);
    }    

    system(command);
    free(command);


    //// READ RESULTS ////

    add_block( 
        data, 
        read_file_block( tmp_filename )
    );

    //// REMOVE TMP FILE ////

    command = calloc( strlen("rm \"\"") + strlen(tmp_filename) + 1, sizeof(char));

    if (NULL == command ) {
        error_log("failed to allocate memory for command `rm`");
        exit(1);
    }

    if (sprintf(command, "rm \"%s\"", tmp_filename) == -1) {
        error_log("failed to concatenate string");
        exit(1);
    }

    system(command);
    free(command);
}
