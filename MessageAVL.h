#ifndef MESSAGEAVL_H_
#define MESSAGEAVL_H_

#include "LinkedList.h"

/*
 * Structure we want to store
 * Key is used to order data
 * Using linked_list to store all signals for that message
*/
struct message_node {
    int key;
    char name[50];
    struct my_list *list;
    int log_mode;
    int count;
};

int data_cmp_msg(void *a, void *b);

void data_print_msg(void *d);

void data_delete_msg(void *d);

void data_copy_msg(void *src, void *dst);

tree* initialize_msg_avl();

#endif //MESSAGEAVL_H_
