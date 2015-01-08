/*
 * datalogger.c
 *
 *  Created on: Jan 19, 2014
 *      Author: Sean
 */


#include "all.h"

struct timeval start_tv, now_tv, new_tv, old_tv;
double prev_system_time;
int headers_logged = 0, renamed = 0;
struct signal_node sig_node_key;
struct signal_node * sig_node;
struct signal_node * data;
struct message_node * msg;
extern sem_t semaphore;
extern FILE *f;
extern char logString[15];
extern tree *msg_tree;
extern int errors;
time_t rawtime, time_of_day;
struct tm * timeinfo;
struct tm t;
sem_t time_semaphore;
// Used for signals
void insert_headers(void *n)
{
	data = (struct signal_node *) n;
	fprintf(f, "%s", data->key);
	if(data->signal->unit != NULL)
	{
		fprintf(f, " %s,", data->signal->unit);
	}
	else
	{
		fprintf(f, ",");
	}
	gettimeofday(&start_tv, NULL);
	sem_init(&time_semaphore, 0, 1);
}

// Only datalog certain headers... (heartbeats)
void insert_headers_messages(void *n)
{
	msg = (struct message_node *) n;
	if(msg->log_mode != 0)
	{
		fprintf(f, "%s", msg->name);
		fprintf(f, ",");
	}
}

void node_to_file(void *n)
{
	data = (struct signal_node *) n;
	if(data == NULL)
	{
		printf("There was a pointer that was NULL. Find me\n");
		exit(1);
	}
	fprintf(f, "%f,", data->value);
}

void msg_to_file(void *n)
{
	msg = (struct message_node *) n;
	if(msg->log_mode != 0)
	{
		fprintf(f, "%i,", msg->count);
	}
}

void data_log(tree *signal_tree)
{
	// The signal to determine if we need to rename the file and change system time
	strcpy(sig_node_key.key, "GPSValidityStatus");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	if(sig_node->signal == NULL || sig_node == NULL)
	{
		printf("There was a pointer that was NULL. Find me\n");
		exit(1);
	}
	gettimeofday(&now_tv, NULL);
	// Prints runtime
	fprintf(f, "%g,", (double) (now_tv.tv_usec - start_tv.tv_usec) / 1000000 + (double) (now_tv.tv_sec - start_tv.tv_sec) + prev_system_time);
	if(sig_node != NULL && sig_node->value >= 1)
	{
		if(renamed == 0)
		{
			renamed = 1;
			rename_log(signal_tree);
		}
	}

	// Prints system time
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	fprintf(f, "%.*s,", 24, asctime (timeinfo));
	fprintf(f, "%i,", errors);
	// Semaphore to protect against reading data that is being changed
	sem_wait(&semaphore);
	explore_tree(msg_tree, msg_to_file);
	explore_tree(signal_tree, node_to_file);
	sem_post(&semaphore);
	fprintf(f, "\n");
}

int rename_log(tree *signal_tree)
{
	char fileName[100] = "/home/cancorder/", value[5];
	// Obtain Y, M, D, H, M, S for tree, store in something local
	int year, month, day, hour, minute, seconds;
	strcpy(sig_node_key.key, "Years");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	year = sig_node->value;
	if(year > 2500 || year < 2000) // If we time traveled to the year 2500, we would surely come in last place... we don't want that
	{							   // But if we time traveled to before 2000... wait, maybe that's not a bad idea...
		renamed = 0;
		return 1;
	}
	sprintf(value, "%d", year);
	strcat(fileName, value);
	strcat(fileName, "-");

	strcpy(sig_node_key.key, "Month");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	month = sig_node->value;
	if(month > 12 || month == 0)
	{
		renamed = 0;
		return 1;
	}
	sprintf(value, "%d", month);
	strcat(fileName, value);
	strcat(fileName, "-");

	strcpy(sig_node_key.key, "Day");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	day = sig_node->value;
	if(day > 31 || day == 0)
	{
		renamed = 0;
		return 1;
	}
	sprintf(value, "%d", day);
	strcat(fileName, value);
	strcat(fileName, "--");

	strcpy(sig_node_key.key, "Hours");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	hour = sig_node->value;
	if(hour > 23)
	{
		renamed = 0;
		return 1;
	}
	sprintf(value, "%d", hour);
	strcat(fileName, value);
	strcat(fileName, "-");

	strcpy(sig_node_key.key, "Minutes");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	minute = sig_node->value;
	if(minute > 60)
	{
		renamed = 0;
		return 1;
	}
	sprintf(value, "%d", minute);
	strcat(fileName, value);
	strcat(fileName, ".csv");

	strcpy(sig_node_key.key, "Seconds");
	sig_node = get_signal(signal_tree, &sig_node_key, sizeof(struct signal_node));
	seconds = sig_node->value;
	if(seconds > 60)
	{
		renamed = 0;
		return 1;
	}

	printf("About to change time");
	/*
	t.tm_year = year-1900;
	t.tm_mon = month - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = minute;
	t.tm_sec = seconds;
	t.tm_isdst = -1;
	time_of_day = mktime(&t);
	new_tv.tv_sec = time_of_day;
	*/

	prev_system_time = (double) (now_tv.tv_usec - start_tv.tv_usec) / 1000000 + (double) (now_tv.tv_sec - start_tv.tv_sec);

	//if(prev_system_time < new_tv.tv_sec)
	//{
		settimeofday(&new_tv, NULL);
		gettimeofday(&start_tv, NULL);
	//}
	/*
	else
	{
		prev_system_time = 0;
		renamed = 0;
		return 1;
	}
	*/
	//gettimeofday(&now_tv, NULL);
	//Final fileName format = 2014-2-1--15-36 (Y-M-D--Hour-Minutes)
	fclose(f);
	// Rename file, reopen it for datalogging
	printf("%s\n", fileName);
	rename(logString, fileName);
	f = fopen(fileName, "a");
	return 0;
}


