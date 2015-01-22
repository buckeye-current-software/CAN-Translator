/*
 * main.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Sean
 */
#include "all.h"

tree *msg_tree;
tree *signal_tree;
sem_t semaphore, can_semaphore;
FILE *f;
char logString[150];
unsigned int errors = 0;
extern int keepRunning;

void my_handler(int dummy) {
    keepRunning = 0;
}

/*
 * Thread dedicated to sending out the CANcorder heartbeat
 */
void *txcanthread(int cansock) {

	struct can_frame txmsg;
	while(keepRunning)
	{
		sleep(1);
		txmsg.can_id = 0xAA; 		// CANcorder heartbeat address
		txmsg.can_dlc = 0; 	// No data associated with the heartbeat

		if(write(cansock, &txmsg, sizeof(struct can_frame)) < 0)
		{
			printf("Error writing heartbeat\n");
		}
	}
	return NULL;
}

void *logthread()
{
	// Generate a default log file name before it is named. log1.csv, log2.csv, etc..
	int logNum = 1;
	char lognumstr[4];
	sprintf(lognumstr, "%d", logNum);
	strcpy(logString, "/home/cancorder/log");
	//strcpy(logString, "log");
	strcat(logString, lognumstr);
	strcat(logString, ".csv");
	while(access(logString, F_OK ) != -1)
	{
		logNum ++;
		sprintf(lognumstr, "%d", logNum);
		strcpy(logString, "/home/cancorder/log");
		//strcpy(logString, "log");
		strcat(logString, lognumstr);
		strcat(logString, ".csv");
	}
	// Open the file with write permissions
	f = fopen(logString, "w");
	if(f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	fprintf(f, "Runtime,System_Time,ErrorFrames,");			// Start inserting headers
	explore_tree(msg_tree, insert_headers_messages);
	explore_tree(signal_tree, insert_headers); 	// Generates headers once (can change to once in so many lines written)
	fprintf(f, "\n");
	while(keepRunning)
	{
		usleep(25000); // Will cause "memory leak" since it can write to the file everything this quickly. Won't crash program
		data_log(signal_tree); // Datalog values in the signal tree
		fflush(f);
	}
	fclose(f);
	return NULL;
}


int main()
{
	int s;

	/*
	struct sigaction sigIntHandler;
	// Event handlers for use in debugging (Catch ctrl + C)
	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	 */
	msg_tree = initialize_msg_avl();		// Initialize trees that will store parsed data from .dbc file
	signal_tree = initialize_signal_avl();

	char *fileName = "/home/cancorder/workspace/CAN_Translator/IOM2014.dbc";			// Your .dbc file
	//char *fileName = "IOM2014.dbc";
	parseFile(fileName);	// Parse the file

	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Error while opening socket");
		return -1;
	}
	sem_init(&semaphore, 0, 1);
	sem_init(&can_semaphore, 0, 1);

	pthread_t interceptor, translator;
	pthread_t txthread, logging;
	pthread_create(&interceptor, NULL, can_interceptor_thread, s);
	pthread_create(&translator, NULL, translate_thread, NULL);
	pthread_create(&txthread, NULL, txcanthread, s);
	pthread_create(&logging, NULL, logthread, NULL);
	sleep(2);

	while(1)
	{
		//Do nothing
	}

	return 0;
}
