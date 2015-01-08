/*
 * SignalAVL.h
 *
 *  Created on: Jan 15, 2014
 *      Author: Sean
 */

#ifndef SIGNALAVL_H_
#define SIGNALAVL_H_

#include "signal.h"
#include <pthread.h>

struct signal_node {
	char key[50];
	struct signal_structure *signal;
	double value;
};

int data_cmp_sig(void *a, void *b);

void data_print_sig(void *d);

void data_delete_sig(void *d);

void data_copy_sig(void *src, void *dst);

tree* initialize_signal_avl();

#endif /* SIGNALAVL_H_ */
