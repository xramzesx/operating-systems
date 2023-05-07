#pragma once

struct Node {
    int pid;
    struct Node * next;
};

typedef struct Node Node;

struct LinkedList {
    struct Node * head;
    struct Node * tail;
};

typedef struct LinkedList LinkedList;

LinkedList * init_linked_list();
void list_append( LinkedList * list, int pid );
void destroy_linked_list(LinkedList * list);