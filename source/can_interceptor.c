/*
 * can_interceptor.c
 *
 *  Created on: Jan 7, 2015
 *      Author: Sean Harrington
 */

#include "all.h"
tree *msg_tree;
tree *signal_tree;
const int canfd_on = 1;
int keepRunning = 1;

int * can_interceptor_thread(int s)
{
	fd_set rdfs;
	char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
	struct iovec iov;
	struct msghdr msg;

	struct canfd_frame frame;
	int nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;

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

	iov.iov_base = &frame;
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;


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
				/*
				pthread_join(txthread, NULL);
				pthread_join(logging, NULL);
				pthread_join(syncthread, NULL);
				delete_tree(signal_tree);
				delete_tree(msg_tree);
				*/
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
	/*
	pthread_join(txthread, NULL); // We want to close the threads before deleting the trees they are accessing
	pthread_join(logging, NULL);
	pthread_join(syncthread, NULL);
	*/
	delete_tree(signal_tree);
	delete_tree(msg_tree);
	free(signal_tree);
	signal_tree = NULL;
	free(msg_tree);
	msg_tree = NULL;
	return 0;
}
