#pragma once
#define MAX_LINE_LENGTH 4096
#define MAX_BLOCK_SIZE BUFSIZ

//// UTILS ////

void message_log( const char * message );
void error_log( const char * error_message );
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

void delete_block ( Data * data, size_t index );
void add_block ( Data *data, void * block );
void * get_block( Data * data, size_t index );

//// MAIN METHODS ////

void word_count ( Data *data, const char *filename );
