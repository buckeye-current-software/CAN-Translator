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

extern sem_t semaphore, mutex;
extern struct can_queue can_read_queue;
struct can_queue translated_queue;
extern time_t startTime;
char mysql_statement[2000], query[200];

void * translate_thread()
{
	struct can_message * can_message_to_translate;
	struct signal_structure * signal;
	uint8_t frameLength;
	uint64_t bitmask = 0;
	struct canfd_frame * frame;

	int i;
	FILE * insertion_file;

	while(1)
	{
		sem_wait(&semaphore);

		if(can_read_queue.head != NULL)
		{
			// Add semaphore to lock down the queue while these two operations occur
			sem_wait(&mutex);
			can_message_to_translate = can_read_queue.head;
			can_read_queue.head = can_message_to_translate->next;
			sem_post(&mutex);

			frame = can_message_to_translate->frame;
			frameLength = frame->len;
			__u8 tempArray[frameLength];
			__u8 origFrameData[frameLength];

			memcpy(&origFrameData, frame->data, frameLength);

			struct list_node* head_signal = can_message_to_translate->can_signals->head;
			signal = head_signal->signal;
			while(head_signal != NULL)
			{
				memcpy(&frame->data, &origFrameData[0], frameLength);
				memcpy(&byteData.U64, &frame->data[0], frameLength);

				// Used for manual file IO entry
				insertion_file = fopen(head_signal->signal->id, "r+");
				if(insertion_file == NULL)
				{
					printf("FAILED TO OPEN FILE!\n");
				}

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

				sem_wait(&semaphore);
				// Determine how the translated data should be interpreted (int, float, double, etc..)
				if(signal->dataType == 1) // Signed int
				{
					if(signal->length <= 8)
					{
						if(signal->length == 8)
						{
							head_signal->value = (double)byteData.I8;
						}
						else
						{
							head_signal->value = (double)((byteData.I8 << (8 - signal->length)) >> (8-signal->length));
						}
					}
					else if(signal->length <= 16)
					{
						if(signal->length == 16)
						{
							head_signal->value = (double)byteData.I16;
						}
						else
						{
							head_signal->value = (double)((byteData.I16 << (16 - signal->length)) >> (16-signal->length));
						}

					}
					else if(signal->length <= 32)
					{
						if(signal->length == 32)
						{
							head_signal->value = (double)byteData.I32;
						}
						else
						{
							head_signal->value = (double)((byteData.I32 << (32 - signal->length)) >> (32-signal->length));
						}
					}
					else
					{
						if(signal->length == 64)
						{
							head_signal->value = (double)byteData.I64;
						}
						else
						{
							head_signal->value = (double)((byteData.I64 << (64 - signal->length)) >> (64-signal->length));
						}
					}
				}
				else if(signal->dataType == 2) // Unsigned int
				{
					if(signal->length <= 8)
					{
						head_signal->value = (double)byteData.U8;

					}
					else if(signal->length <= 16)
					{
						head_signal->value = (double)byteData.U16;

					}
					else if(signal->length <= 32)
					{
						head_signal->value = (double)byteData.U32;
					}
					else
					{
						head_signal->value = (double)byteData.U64;
					}
				}
				else if(signal->dataType == 3) // Float
				{
					if(signal->length != 32)
					{
						//
					}
					head_signal->value = (double)byteData.FLOAT;
					if(head_signal->value != head_signal->value)
					{
						// The float is NaN. Set to 0.
						head_signal->value = 0;
					}
				}
				else //Double
				{
					head_signal->value = byteData.DOUBLE;
				}
				sem_post(&semaphore);


				if(head_signal->next != NULL)
				{
					signal = head_signal->next->signal;
				}

				rewind(insertion_file);
				double time = 0.0;	// Will change as needed
				double value = head_signal->value;
				fwrite(&time, sizeof time, 1, insertion_file);	
				fwrite(&value, sizeof value, 1, insertion_file);
				fclose(insertion_file);
				head_signal = head_signal->next;
			}
			free(can_message_to_translate);
		}
	}
}

