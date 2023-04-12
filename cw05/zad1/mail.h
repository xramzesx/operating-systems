#pragma once

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
);

int parse_month( const char * month );

char * date_to_string( Date date );

//// CONSTRUCTORS ////

Mail * init_mail();
Node * init_node();

//// DESTRUCTORS ////

void destroy_mail(Mail * mail);
void destroy_node(Node * node);

//// COMPARATORS ////

int cmp_mail_date( 
    const void * va, 
    const void * vb
);
int cmp_mail_sender( 
    const void * va, 
    const void * vb
);

///// MAIN /////

int show_mail( char * option );
int send_mail( 
    char * email, 
    char * title, 
    char * content 
);