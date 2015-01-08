/*
 * SignalAVL.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Sean
 */
#include "all.h"



int data_cmp_sig(void *a, void *b)
{
    struct signal_node *aa = (struct signal_node *) a;
    struct signal_node *bb = (struct signal_node *) b;

    // Protect against NULL pointer
    // It could generally never happened
    if (!aa || !bb)
        return 0;
    //printf("Key 1: %s ---- Key 2: %s\n", aa->key, bb->key);
    /*if(strstr(aa->key, bb->key))
    {
    	return 0;
    }*/
    return strcmp(aa->key, bb->key);

}

// Function that dumps data structure
void data_print_sig(void *d)
{
	/*
	 * Mostly deleted. Only used to initialize a tree
	 */
    struct signal_node *dd = (struct signal_node *) d;
    if (dd)
    	printf("{ %s -> %s }\n", " ", dd->signal->id);
}

// Function that delete a data structure
void data_delete_sig(void *d)
{
    struct signal_node *dd = (struct signal_node *) d;

    if (dd) {
    	dd->signal = NULL;
        free(dd);
        dd = NULL;
    }
}

// Function that copy data structure
void data_copy_sig(void *src, void *dst)
{
    struct signal_node *s = (struct signal_node *) src;
    struct signal_node *d = (struct signal_node *) dst;

    memcpy(d->key,s->key,50);
    d->signal = s->signal;
}

tree* initialize_signal_avl()
{
	tree *signal_tree = NULL;
	signal_tree = init_dictionnary(data_cmp_sig, data_print_sig, data_delete_sig, data_copy_sig);
	return signal_tree;
}

