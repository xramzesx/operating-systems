#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mail.h"

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

void print_mail_header(Mail * mail) {
    char * date = date_to_string(mail->date);

    printf(
        "%s\t%s\t%s\n", 
        date, 
        mail->sender, 
        mail->subject
    );

    free(date);
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

//// DESTRUCTORS ////

void destroy_mail(Mail * mail) {

    if (mail == NULL) return;

    if (mail->sender != NULL)
        free(mail->sender);
    if (mail->subject != NULL)
        free(mail->subject);

    free(mail);
}

void destroy_node(Node * node) {
    free(node->mail);
    free(node);
}

//// COMPARATORS ////

int cmp_mail_date( const void * va, const void * vb) {
    Mail * ma = (Mail *) va;
    Mail * mb = (Mail *) vb;
    
    Date * a = &ma->date;
    Date * b = &mb->date;

    /// DATE ///
    if (a->year   != b->year)   return a->year - b->year;
    if (a->month  != b->month)  return a->month - b->month;
    if (a->day    != b->day)    return a->day - b->day;
    
    /// TIME ///
    if (a->hour   != b->hour)   return a->hour - b->hour;
    if (a->minute != b->minute) return a->minute - b->minute;
    if (a->second != b->second) return a->second - b->second;

    return 0;
}

int cmp_mail_sender( const void * va, const void * vb) {
    Mail * ma = (Mail *) va;
    Mail * mb = (Mail *) vb;

    return strcmp( ma->sender, mb->sender );
}

///// MAIN /////

int show_mail( char * option ) {

    FILE * mail_source = popen("mail -p", "r");
    
    if (mail_source == NULL) {
        perror("failed to open read pipe");
        return 2;
    }

    char line[BUFSIZ];

    Node * mail_nodes = init_node();
    Node * node = mail_nodes;

    int counter = 0;

    while ( fgets(line, BUFSIZ, mail_source) ) {

        if (strncmp(line, "Subject:", 8) == 0 && strlen( node->mail->subject ) == 0) {
            strcpy(node->mail->subject, &line[8]);
            node->mail->subject[strlen(node->mail->subject) - 1] = 0;
            counter++;
        }

        if (strncmp(line, "Date:", 5) == 0 && node->mail->date.year == 0) {
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
            strcpy(node->mail->sender, &line[5]);
            node->mail->sender[strlen(node->mail->sender) - 1] = 0;

            node-> next = init_node();
            node = node->next;
        }

    }

    pclose(mail_source);

    //// LINKED LIST TO ARRAY ////

    Mail * mails = calloc( counter, sizeof(Mail));    
    Node * tmp;
    node = mail_nodes;

    for ( int i = 0; i < counter; i++ ) {
        mails[i] = *node->mail;
        
        tmp = node;
        node = node->next;
        
        destroy_node(tmp);
    }

    if (node != NULL) {
        free(node);
    }

    //// GET PROPER ORDER ////

    if (
        strcmp(option, "sender") == 0 || 
        strcmp(option, "nadawca") == 0
    ) {
        qsort( mails, counter, sizeof(Mail), cmp_mail_sender );
    } else if (
        strcmp(option, "data") == 0 || 
        strcmp(option, "date") == 0
    ) {
        qsort( mails, counter, sizeof(Mail), cmp_mail_date );
    } else {
        perror("Invalid parameter");
        printf("[default order] \n");
    }
    for (int i = 0; i < counter; i++) {
        print_mail_header(&mails[i]);
    }

    free(mails);

    return 0;
}

int send_mail( char * email, char * title, char * content ) {

    return 0;
}