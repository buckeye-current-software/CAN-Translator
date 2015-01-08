/*
 * translator.c
 *
 *  Created on: Jan 18, 2014
 *      Author: Sean
 */

#include "all.h"

union byteData
{
  uint8_t U8;
  int8_t I8;
  uint16_t U16;
  int16_t I16;
  uint32_t U32;
  int32_t I32;
  uint64_t U64;
  int64_t I64;
  float_t FLOAT;
  double_t DOUBLE;
} byteData;

extern sem_t semaphore;

int translate(tree *message_tree, tree *signal_tree, struct canfd_frame *frame) {

	uint8_t frameLength = frame->len;
	int msgID = (int)frame->can_id;
	int i;								// Index for loops as needed
	__u8 tempArray[frameLength];		// Used to flip data depending on endianness
	uint64_t bitmask = 0;
	__u8 origFrameData[frameLength];
	memcpy(&origFrameData, &frame->data[0], frameLength);

	struct message_node msg_node_key;
	struct message_node * msg_node;

	struct signal_node sig_node_key;
	struct signal_node * sig_node;
	struct signal_structure *signal;
	struct list_node *node;
	msg_node_key.key = msgID;
	/*
	 * Only translate incoming messages if they are in the message tree and the length
	 * of the message is greater than 0 bytes (not empty)
	 */
	if(is_present(message_tree, &msg_node_key) && frameLength > 0)
	{
		msg_node = get_message(message_tree, &msg_node_key, sizeof(struct message_node));

		// Increase count of this message received
		msg_node->count++;

		node = msg_node->list->head;
		// Protect against the first list node (signal) being NULL. Serious issue occurred, check message_tree for validity
		if(node == NULL)
		{
			return 1;
		}
		signal = node->signal;

		while(node != NULL) // While there is a signal in the message we haven't translated yet...
		{
			memcpy(&frame->data, &origFrameData[0], frameLength);
			memcpy(&byteData.U64, &frame->data[0], frameLength);

			strcpy(sig_node_key.key, signal->id);
			sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));

			if(signal->byteOrder == 0) // If little endian, the data needs reversed
			{
				// .dbc files encode start bits differently for big endian -- converts startBit
				signal->startBit = 64 - (8*(signal->startBit/8)) - ((8-((signal->startBit+1)%8))%8) - signal->length;
				//Reverse the byte array or start index at end and decrement
				for(i = 0; i < 8; i++)
				{
					tempArray[(frameLength-1)-i] = frame->data[i];
				}
				memcpy(&frame->data,&tempArray[0], frameLength);
			}

			memcpy(&byteData.U64, &frame->data[0], frameLength); // Obtain data and store in union
			byteData.U64 = byteData.U64 >> signal->startBit; // Remove data that doesn't correspond to the signal we are looking at
			//Create bitmask
			bitmask = 0;
			for(i = 0; i < signal->length; i ++) {
				bitmask = (bitmask | (1L << i));
			}
			byteData.U64 = byteData.U64 & bitmask;

			signal->dataType = sig_node->signal->dataType; // Due to signals.dataType being different between msg_tree and sig_tree


			sem_wait(&semaphore);
			// Determine how the translated data should be interpreted (int, float, double, etc..)
			if(signal->dataType == 1) // Signed int
			{
				if(signal->length <= 8)
				{
					if(signal->length == 8)
					{
						sig_node->value = (double)byteData.I8;
					}
					else
					{
						sig_node->value = (double)((byteData.I8 << (8 - signal->length)) >> (8-signal->length));
					}
				}
				else if(signal->length <= 16)
				{
					if(signal->length == 16)
					{
						sig_node->value = (double)byteData.I16;
					}
					else
					{
						sig_node->value = (double)((byteData.I16 << (16 - signal->length)) >> (16-signal->length));
					}

				}
				else if(signal->length <= 32)
				{
					if(signal->length == 32)
					{
						sig_node->value = (double)byteData.I32;
					}
					else
					{
						sig_node->value = (double)((byteData.I32 << (32 - signal->length)) >> (32-signal->length));
					}
				}
				else
				{
					if(signal->length == 64)
					{
						sig_node->value = (double)byteData.I64;
					}
					else
					{
						sig_node->value = (double)((byteData.I64 << (64 - signal->length)) >> (64-signal->length));
					}
				}
			}
			else if(signal->dataType == 2) // Unsigned int
			{
				if(signal->length <= 8)
				{
					sig_node->value = (double)byteData.U8;

				}
				else if(signal->length <= 16)
				{
					sig_node->value = (double)byteData.U16;

				}
				else if(signal->length <= 32)
				{
					sig_node->value = (double)byteData.U32;
				}
				else
				{
					sig_node->value = (double)byteData.U64;
				}
			}
			else if(signal->dataType == 3) // Float
			{
				if(signal->length != 32)
				{
					//
				}
				sig_node->value = (double)byteData.FLOAT;
			}
			else //Double
			{
				sig_node->value = byteData.DOUBLE;
			}
			sem_post(&semaphore);


			if(node->next != NULL)
			{
				signal = node->next->signal;
			}
			node = node->next;
		}
	}
	return 0;
}

