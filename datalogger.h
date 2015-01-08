/*
 * datalogger.h
 *
 *  Created on: Jan 19, 2014
 *      Author: Sean
 */

#ifndef DATALOGGER_H_
#define DATALOGGER_H_

#include "avl.h"

/*
 * Inserts the names and units of all signals into the top line of the datalogging file
 */
void insert_headers(void *n);

/*
 * Inserts the names of messages that have a log_mode other than 0.
 */
void insert_headers_messages(void *n);

/*
 * Takes a specific node and datalogs it's value into the file. Used in conjunction with
 * avl.h's explore_tree function
 */
void node_to_file(void *n);

/*
 * Takes a message node and datalogs the count value stored in the message. Used in
 * conjunction with avl.h's explore_tree function.
 */
void msg_to_file(void *n);

/*
 * Handles the overall datalogging process. Calls other datalogging functions.
 * Parameters:
 * 		signal_tree - the tree to datalog (should be a signal tree not a message tree)
 */
void data_log(tree *signal_tree);

/*
 * Renames the file and changes system time when GPSValidityStatus == 1
 */
int rename_log(tree *signal_tree);

#endif /* DATALOGGER_H_ */
