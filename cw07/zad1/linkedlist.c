#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "linkedlist.h"

LinkedList * init_linked_list() {
    LinkedList * result = (LinkedList *) calloc(1, sizeof(LinkedList));
    result->head = NULL;
    result->tail = NULL;
}

void list_append( LinkedList * list, int pid ) {
    Node * result = (Node *)calloc(1, sizeof(Node));
    result->pid = pid;
    result->next = NULL;

    if (list->head == NULL) {
        list->head = result;
        list->tail = result;
        return;
    }

    list->tail->next = result;
    list->tail = list->tail->next;
}

void destroy_linked_list(LinkedList * list) {
    Node * node = list->head;

    while (node != NULL) {
        kill(node->pid, SIGINT);
        Node * tmp = node;
        node = node->next;
        free(tmp);
    }

    free(list);
}