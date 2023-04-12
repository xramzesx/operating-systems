#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mail.h"

int main (int argc, char **argv) {

    if ( argc != 2 && argc != 4) {
        perror("invalid argument count");
        return 1;
    }
    
    return argc == 2 
        ? show_mail(
            argv[1]
        )
        : send_mail(
            argv[1], 
            argv[2], 
            argv[3]
        );
}