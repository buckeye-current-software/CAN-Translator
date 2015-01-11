/*
 * LinkedList.h
 *
 *  Created on: Jan 12, 2014
 *      Author: Sean
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include "signal.h"

//Stores signals
struct list_node
{
  struct signal_structure *signal;
  struct list_node* next;
};

struct can_message
{
	struct canfd_frame * frame;
	struct can_message * next;
	struct my_list * can_signals;
};


struct my_list
{
  struct list_node* head;
  struct list_node* tail;
};

struct can_queue
{
	struct can_message * head;
	struct can_message * tail;
};


struct signal_structure* list_add_element( struct my_list*, struct signal_structure*);
struct my_list* list_remove_element( struct my_list*);


struct my_list* list_new(void);
struct my_list* list_free( struct my_list* );

void list_print( const struct my_list* );
void list_print_element(const struct list_node* );

#endif /* LINKEDLIST_H_ */
