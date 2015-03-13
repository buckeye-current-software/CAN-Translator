
/* 
   Parser Outline
   Version 0.1, Buckeye Current 
*/
#include "all.h"

extern tree *msg_tree;
int maxSignalSize;

void parseFile(char *fileName)
{
	maxSignalSize = 0;
	FILE *file;
	/* Open file handler using fileName in read mode */
	file = fopen(fileName, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Can't open %s\n", fileName);
		exit(1);		
	}

	char buf[BUFSIZ], messageID[10], messageName[50], tmp[50], startBit[3], length[3], byteOrder[2], dataType[2];
	int index, index2 = 0, tmpLength; 							// Variables used to store signalID's into smaller arrays
	int first_insert_skipped = 0;     // Skips inserting the first message since no signals have
																// been associated to it yet (it's added later)
	
	struct my_list* signal_linked_list = NULL; 	// Linked list of signals all in a single message
	struct message_node msg, tempNode;
	struct message_node * msgnode_to_edit;
	struct list_node * signode_to_edit;
	struct message_node * message;		// Signal nodes that are edited after in tree
	struct signal_structure sig; 				// An actual signal structure
	
	/* Read a line in at a time */
	while(fgets(buf,BUFSIZ,file) != NULL)
	{
		if(strstr(buf, "BO_")-buf == 0)
		{
			if(first_insert_skipped == 1)
			{
				// add current linked_list and message to Message AVL tree
				msg.list = signal_linked_list; //Getting a warning before about "assignment from incompatible pointer type"
				insert_elmt(msg_tree, &msg, sizeof(struct message_node));
			}
			first_insert_skipped = 1; // Start adding messages to tree after first msg in file found and skipped

			index = 4;
			while(buf[index] != ' ')
			{
				messageID[index-4] = buf[index];
				index++;
			}
			messageID[index-4] = '\0';
			msg.key = atoi(messageID);
			index++;
			index2 = 0;
			while(buf[index] != ':')
			{
				messageName[index2] = buf[index];
				index++;
				index2++;
			}
			messageName[index2] = '\0';
			strcpy(msg.name, messageName);
			msg.count = 0;
			msg.log_mode = 0;
			signal_linked_list = list_new();
		}

		else if(strstr(buf, "CM_ SG_") != NULL)
		{
			msg.list = signal_linked_list; //Getting a warning before about "assignment from incompatible pointer type"
			insert_elmt(msg_tree, &msg, sizeof(struct message_node));

			int id = 0;
			index = 8;
			while(buf[index] != ' ')
			{
				tmp[index-8] = buf[index];
				index++;
			}
			tmp[index-8] = '\0';
			id = atoi(tmp);
			tempNode.key = id;
			index += 1;
			index2 = 0;
			while(buf[index] != ' ')
			{
				tmp[index2] = buf[index];
				index++;
				index2++;
			}
			tmp[index2] = '\0';

			msgnode_to_edit = get_message(msg_tree, &tempNode, sizeof(struct message_node));
			signode_to_edit = msgnode_to_edit->list->head;
			while(strcmp(signode_to_edit->signal->id,tmp) != 0)
			{
				signode_to_edit = signode_to_edit->next;
			}
			index += 2;
			index2 = 0;
			while(buf[index] != ',')
			{
				tmp[index2] = buf[index];
				index++;
				index2++;
			}
			tmp[index2] = '\0';
			signode_to_edit->signal->warnStart = atoi(tmp);

			index ++;
			index2 = 0;
			while(buf[index] != ']')
			{
				tmp[index2] = buf[index];
				index++;
				index2++;
			}
			tmp[index2] = '\0';
			signode_to_edit->signal->warnEnd = atoi(tmp);
		}

		else if(strstr(buf, "SG_") != NULL)
		{
			index = 5;
			index2 = 0;
			// Store index 5 through * as signal ID in Bike
			while(buf[index] != ' ')
			{
				tmp[index-5] = buf[index];
				index++;
			}
			tmp[index-5] = '\0';

			// Allocate smallest char array possible to store signal_ID
			tmpLength = strlen(tmp)+1;
			if(tmpLength > maxSignalSize)
			{
				maxSignalSize = tmpLength;
			}

			sig.unit[0] = '\0';
			strcpy(sig.id, tmp);
			
			// Move index +3 to skip useless dbc formatting stuff
			index = index + 3;
			// Store start bit
			while(buf[index] != '|')
			{
				startBit[index2] = buf[index];
				index++;
				index2++;
			}
			startBit[index2] = '\0';
			sig.startBit = atoi(startBit); // Converts string to int
			index2 = 0;
			index++;

			// Store length 
			while(buf[index] != '@')
			{
				length[index2] = buf[index];
				index++;
				index2++;
			}
			length[index2] = '\0';
			sig.length = atoi(length); 
			index2 = 0;
			index++;
			
			// Store byte order
			byteOrder[0] = buf[index];
			index++;
			byteOrder[1] = '\0';
			sig.byteOrder = atoi(byteOrder);
			
			// Store data type
			dataType[0] = buf[index];
			index++;
			dataType[1] = '\0';

			// Find what the dataType string equals and assign correct integer to sig.dataType

			if(dataType[0] == '-')
			{
				sig.dataType = 1; // Represents signed int
			}
			else
			{
				sig.dataType = 2; // Represents unsigned int
			}

			// Move to the signal's min, max value and store it
			while(buf[index] != '[')
			{
				index ++;
			}
			index ++;
			index2 = 0;
			while(buf[index] != '|')
			{
				tmp[index2] = buf[index];
				index2++;
				index++;
			}
			tmp[index2] = '\0';
			sig.okStart = atoi(tmp);

			index++;
			index2 = 0;
			while(buf[index] != ']')
			{
				tmp[index2] = buf[index];
				index2++;
				index++;
			}
			tmp[index2] = '\0';
			sig.okEnd = atoi(tmp);
			// Move to the signal's unit and store it
			while(buf[index] != '"')
			{
				index ++;
			}
			index ++;
			index2 = 0;
			while(buf[index] != '"')
			{
				tmp[index2] = buf[index];
				index2++;
				index++;
			}
			tmp[index2] = '\0';

			if(index2 > 0)
			{
				strcpy(sig.unit, tmp);
			}

			// Add signal to linked_list
			list_add_element(signal_linked_list, &sig);

			// Add signal to signal node, add signal node to AVL tree
		}

		// This signifies that the data type for a signal stored earlier is wrong
		else if(strstr(buf, "SIG_VALTYPE_ ") != NULL)
		{
			int nameIndex = 0;
			index = 13;
			index2 = 0;
			while(buf[index] != ' ')
			{
				tmp[index2] = buf[index];
				index++;
				index2++;
			}
			tmp[index2] = '\0';
			tempNode.key = atoi(tmp);

			index++;
			while(buf[index] != ' ')
			{
				tmp[nameIndex] = buf[index];
				index++;
				nameIndex++;
			}
			tmp[nameIndex] = '\0';
			//strcpy(sig_node.key, tmp);
			//signode_to_edit = get_signal(signal_tree, &sig_node, sizeof(struct signal_node));
			message = get_message(msg_tree, &tempNode,sizeof(struct message_node));
			signode_to_edit = message->list->head;
			while(strcmp(signode_to_edit->signal->id,tmp) != 0) // This is not right.
			{
				signode_to_edit = signode_to_edit->next;
			}

			index = index + 3;
			if(buf[index] == '1') 	//Change data type to float
			{
				signode_to_edit->signal->dataType = 3;
			}
			else					// Change data type to double
			{
				signode_to_edit->signal->dataType = 4;
			}
		}

	}
	// Insert last message node into the message tree
	msg.list = signal_linked_list;
	insert_elmt(msg_tree, &msg, sizeof(struct message_node));

	//fprintf(stdout, "Message Tree: \n"); // Used for debugging. Make sure everything is in the tree
	//print_tree(msg_tree);
	fclose(file);
}
