/*
 * main.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Sean
 */
#include "all.h"

MYSQL *con;

tree *msg_tree;
sem_t semaphore, can_semaphore, mutex;
FILE *f;
char logString[150];
unsigned int errors = 0;
extern int keepRunning;
time_t startTime;

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


int main()
{
	startTime = time(0);
	int s;
	printf("MySQL client version: %s\n", mysql_get_client_info());

	con = mysql_init(NULL);

	if(con == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}

	if(mysql_real_connect(con, "localhost", "cancorder", "current", "CANCorder", 0, NULL, 0) == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		exit(1);
	}
	/*
	struct sigaction sigIntHandler;
	// Event handlers for use in debugging (Catch ctrl + C)
	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	 */
	msg_tree = initialize_msg_avl();		// Initialize trees that will store parsed data from .dbc file
	//signal_tree = initialize_signal_avl();

	char *fileName = "RW3.dbc";			// Your .dbc file
	//char *fileName = "IOM2014.dbc";
	parseFile(fileName);	// Parse the file

	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Error while opening socket");
		return -1;
	}
	sem_init(&mutex, 0, 1);
	sem_init(&can_semaphore, 0, 1);
	sem_init(&semaphore, 0, 0);

	pthread_t interceptor, translator;
	pthread_t txthread;
	pthread_create(&interceptor, NULL, can_interceptor_thread, s);
	pthread_create(&translator, NULL, translate_thread, NULL);
	pthread_create(&txthread, NULL, txcanthread, s);
	pthread_join(&interceptor, NULL);
	pthread_join(&translator, NULL);
	pthread_join(&txthread, NULL);

	return 0;
}
