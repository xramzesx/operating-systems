#include <stdio.h>
#include <stdlib.h>
#include "lib.h"


int main () {
    /// a) ///
    Data *data = generate_data(10, 0);

    printf("a) success\n   Initial size:\n   - capacity = %ld\n   - size = %ld\n", data->capacity, data->size);
    /// b) ///
    word_count(data, "file1.txt");
    word_count(data, "file2.txt");
    printf("b) success\n");
    
    /// c) ///
    char * result = (char *) get_block(data, 0);  
    
    if ( NULL != result )
        printf("c) success:\n%s\n", result);

    /// d) ///
    
    delete_block(data, 0);
    
    result = (char *) get_block(data, 0);
    
    if (NULL == result)
        printf("d) success\n");

    result = (char *) get_block(data, 1);

    if ( NULL != result )
        printf("- file2:\n%s\n", result);


    /// e) ///
    destroy_data(data);
    data = NULL;

    printf("[log] test ends successfully\n");
    return 0;
}