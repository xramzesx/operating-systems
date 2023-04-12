#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define P_READ 0
#define P_WRITE 1

#define MAX_SUBJECT_SIZE 1024
#define MAX_SENDER_SIZE  512

//// STRUCTS ////

struct Date {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};

typedef struct Date Date;

struct Mail {
    char * subject;
    char * sender;
    Date date;
};

typedef struct Mail Mail;

///// LINKED LIST /////

struct Node {
    Mail * mail;
    struct Node * next;
};

typedef struct Node Node;

///// UTILS /////

char * substring( 
    char * str,
    int index,
    int len
) {
    char * result = calloc( len + 1, sizeof(char) );
    memcpy(result, &str[index], len);
    result[len] = 0;

    return result;
}

int parse_month( const char * month ) {
    if (strcmp("Jan", month) == 0) return 1;
    if (strcmp("Feb", month) == 0) return 2;
    if (strcmp("Mar", month) == 0) return 3;
    if (strcmp("Apr", month) == 0) return 4;
    if (strcmp("May", month) == 0) return 5;
    if (strcmp("Jun", month) == 0) return 6;
    if (strcmp("Jul", month) == 0) return 7;
    if (strcmp("Aug", month) == 0) return 8;
    if (strcmp("Sep", month) == 0) return 9;
    if (strcmp("Oct", month) == 0) return 10;
    if (strcmp("Nov", month) == 0) return 11;
    if (strcmp("Dec", month) == 0) return 12;
    return 0;
}

char * date_to_string( Date date ) {
    char * result = calloc( strlen("yy-mm-dd hh:mm:ss") +1, sizeof(char));
    
    sprintf(
        result, 
        "%02d-%02d-%02d %02d:%02d:%02d", 
        date.year, date.month,  date.day, 
        date.hour, date.minute, date.second
    );

    return result;
}

//// CONSTRUCTORS ////

Mail * init_mail() {
    Mail * mail = calloc(1, sizeof(Mail));

    mail->subject = calloc(MAX_SUBJECT_SIZE, sizeof(char));
    mail->sender = calloc(MAX_SENDER_SIZE, sizeof(char));
    
    return mail;
}

Node * init_node() {
    Node * node = calloc(1, sizeof(Node));
    node->next = NULL;
    node->mail = init_mail();
    return node;
}


///// MAIN /////

int show_mail( char * option ) {

    FILE * mail_source = popen("mail -p", "r");
    char line[BUFSIZ];

    Node * mail_nodes = init_node();
    Node * node = mail_nodes;

    int counter = 0;

    while ( fgets(line, BUFSIZ, mail_source) ) {

        if (strncmp(line, "Subject:", 8) == 0) {
            sscanf(line, "Subject: %s\n", node->mail->subject);
            counter++;
        }

        if (strncmp(line, "Date:", 5) == 0) {
            char _weekday[4];
            char month[4];
            
            sscanf(
                line, 
                "Date: %3s, %d %3s %d %d:%d:%d",
                _weekday,  
                &node->mail->date.day,
                month,
                &node->mail->date.year,
                &node->mail->date.hour,
                &node->mail->date.minute,
                &node->mail->date.second
            );

            node->mail->date.month = parse_month(month);
        }

        if (strncmp(line, "From:", 5) == 0) {
            sscanf(line, "From: %s\n", node->mail->sender);

            node-> next = init_node();
            node = node->next;
        }

    }

    Mail ** mails = calloc( counter, sizeof(Mail));

    node = mail_nodes;

    for ( int i = 0; i < counter; i++ ) {
        mails[i] = node->mail;

        node = node->next;
    }

    for (int i = 0; i < counter; i++) {
        printf("%s\t%s\t%s\n",date_to_string(mails[i]->date) , mails[i]->sender, mails[i]->subject);
    }


    pclose(mail_source);

    return 0;
}

int send_mail( char * email, char * title, char * content ) {

    return 0;
}

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