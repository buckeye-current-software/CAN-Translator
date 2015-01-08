/*
 * translator.h
 *
 *  Created on: Jan 18, 2014
 *      Author: Sean
 */

#ifndef TRANSLATOR_H_
#define TRANSLATOR_H_

#include "avl.h"

/*
 * Translates a can frame and stores the translated values into the correct
 * signals to be datalogged later
 *
 * Params:
 * 		message_tree - Message tree to see if incoming can frame is to be data logged
 * 		signal_tree - Signal tree with all possible signals
 * 		frame - The can frame read from  the CAN bus
 */
int translate(tree *message_tree, tree *signal_tree, struct canfd_frame *frame);

#endif /* TRANSLATOR_H_ */
