#include "all.h"

// Function that compares two struct data
int data_cmp_msg(void *a, void *b)
{
    struct message_node *aa = (struct message_node *) a;
    struct message_node *bb = (struct message_node *) b;

    // Protect against NULL pointer
    // It could generally never happened
    if (!aa || !bb)
        return 0;

    return aa->key - bb->key;
}

// Function that dumps data structure
void data_print_msg(void *d)
{
    struct message_node *dd = (struct message_node *) d;

    if (dd)
        printf("{ %s => %s }\n", " ", " ");
}

// Function that delete a data structure
void data_delete_msg(void *d)
{
    struct message_node *dd = (struct message_node *) d;

    if (dd) {
    	//list_free(dd->list);
    	free(dd->list);
    	dd->list = NULL;
        free(dd);
        dd = NULL;
    }
}

// Function that copy data structure
void data_copy_msg(void *src, void *dst)
{
    struct message_node *s = (struct message_node *) src;
    struct message_node *d = (struct message_node *) dst;

    d->key = s->key;
    memcpy(d->name,s->name,50);
    d->list = s->list;
    d->log_mode = s->log_mode;
    d->count = s->count;

}

tree* initialize_msg_avl()
{
	tree *message_tree = NULL;
	message_tree = init_dictionnary(data_cmp_msg, data_print_msg, data_delete_msg, data_copy_msg);
	return message_tree;
}
