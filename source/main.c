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
const int canfd_on = 1;
int keepRunning = 1;
unsigned int errors = 0;

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

void *syncthread()
{
	while(keepRunning)
	{
		fsync(fileno(f));
		sleep(2);
	}
	return NULL;

}
int main()
{
	struct sigaction sigIntHandler;
	// Event handlers for use in debugging (Catch ctrl + C)
	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	msg_tree = initialize_msg_avl();		// Initialize trees that will store parsed data from .dbc file
	signal_tree = initialize_signal_avl();

	char *fileName = "/home/cancorder/IOM2014.dbc";			// Your .dbc file
	//char *fileName = "IOM2014.dbc";
	parseFile(fileName);	// Parse the file

	fd_set rdfs;
	char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
	struct iovec iov;
	struct msghdr msg;

	struct canfd_frame frame;
	int s, nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Error while opening socket");
		return -1;
	}
	strcpy(ifr.ifr_name, "can0" );
	ioctl(s, SIOCGIFINDEX, &ifr);
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on));
	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		printf("Problem binding socket");
		return 1;
	}

	sem_init(&semaphore, 0, 1);
	sem_init(&can_semaphore, 0, 1);
	pthread_t txthread, logging, sync;
	pthread_create(&txthread, NULL, txcanthread, s);
	pthread_create(&logging, NULL, logthread, NULL);
	sleep(2);
	pthread_create(&sync, NULL, syncthread, NULL);

	unsigned long msgsRecv = 0;

	iov.iov_base = &frame;
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;

	while(keepRunning){
		FD_ZERO(&rdfs);
		FD_SET(s, &rdfs);
		if (FD_ISSET(s, &rdfs))
		{
			iov.iov_len = sizeof(frame);
			msg.msg_namelen = sizeof(addr);
			msg.msg_controllen = sizeof(ctrlmsg);
			msg.msg_flags = 0;
			nbytes = recvmsg(s, &msg, 0);

			//Don't allow the translation of bad reads
			if (nbytes < sizeof(struct can_frame))
			{
				fprintf(stderr, "read: incomplete CAN frame\n");
				return 1;
			}
			if (nbytes < 0)
			{
				perror("Close socket?");
				close(s);
				pthread_join(txthread, NULL);
				pthread_join(logging, NULL);
				pthread_join(syncthread, NULL);
				delete_tree(signal_tree);
				delete_tree(msg_tree);
				return 1;
			}
			if (nbytes > 0)
			{
				//printf("%lu\n", msgsRecv);	// Debugging the receiving of messages from CAN bus.
				//msgsRecv++;
				translate(msg_tree, signal_tree, &frame);
			}
		}
	}
	close(s);
	pthread_join(txthread, NULL); // We want to close the threads before deleting the trees they are accessing
	pthread_join(logging, NULL);
	pthread_join(syncthread, NULL);

	delete_tree(signal_tree);
	delete_tree(msg_tree);
	free(signal_tree);
	signal_tree = NULL;
	free(msg_tree);
	msg_tree = NULL;
	return 0;
}
