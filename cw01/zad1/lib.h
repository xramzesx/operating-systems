#pragma once
#define MAX_LINE_LENGTH 4096
#define MAX_BLOCK_SIZE BUFSIZ

//// UTILS ////

void message_log( const char * message );
void error_log( const char * error_message );

void time_log( 
    struct timespec ts_start,  
    struct timespec ts_end,
    struct tms tms_start,
    struct tms tms_end 
);

void to_lower(char * str);

//// STRUCTURE ////

struct Data {
    void ** blocks;
    size_t capacity;
    size_t size;
};

typedef struct Data Data;

//// STRUCTURE METHODS ////

Data * generate_data (size_t capacity, size_t size);
void destroy_data ( Data * data );

int delete_block ( Data * data, size_t index );
int add_block ( Data *data, void * block );
void * get_block( Data * data, size_t index );

//// MAIN METHODS ////

int word_count ( Data *data, const char *filename );
