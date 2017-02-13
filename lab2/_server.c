Skip to content
This repository
Search
Pull requests
Issues
Gist
 @cho8
 Sign out
 Watch 1
  Star 0
 Fork 0 Simpleple/ECE420Lab
 Code  Issues 0  Pull requests 0  Projects 0  Wiki  Pulse  Graphs
Branch: master Find file Copy pathECE420Lab/Development Kit Lab2/server.c
09f1680  a day ago
@Simpleple Simpleple resultes
2 contributors @Simpleple @restart365
RawBlameHistory
Executable File  153 lines (122 sloc)  3.43 KB
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "timer.h"

#define STR_LEN 50   // max string length
#define X 	    1000 // number of threads

// array size
int n;

// struct that contains information a thread may need
// including file descriptor and thread number
struct FD_Thread {
	int fd;
	int thread;
};

// array to record time for each thread
double times[X];

// the array
char **theArray;

pthread_mutex_t mutex;

// thread function
void *ServerEcho(void *args)
{
	// info passed from main thread
	struct FD_Thread *fd_thread = (struct FD_Thread*)args;

	// read a string from client into str
	char str[STR_LEN];
	read(fd_thread->fd, str, STR_LEN);

	// record the moment of start and end
	double start, end;
	GET_TIME(start);

	// split the string, the first int is the array position
	// and the second is the operation (read/write)
	int pos, mode;
	sscanf(str, "%d %d", &mode, &pos);

	// lock the mutex
	pthread_mutex_lock(&mutex);

	// critical section
	// write to the array
	if(mode == 1) {
		sprintf(theArray[pos], "String %d has been modifed by a write request", pos);
	}
	// read the array
	if(mode == 0) {
		char * temp;
		temp = theArray[pos];
	}

	// unlock the mutex
	pthread_mutex_unlock(&mutex);

	// record the end time
	GET_TIME(end);

	// write the string at pos back to client
	write(fd_thread->fd, theArray[pos], STR_LEN);

	// record the time used
	times[fd_thread->thread] = end - start;

	// close the connection
	close(fd_thread->fd);
}

// main thread
int main(int argc, char* argv[])
{
	// read the port number and array size from arguments
	int port_num = atoi(argv[1]);
	n = atoi(argv[2]);

	// initialize the array
	theArray = (char **) malloc(sizeof(char *) * n);

	int i;
	for(i = 0; i < n; i++) {
		theArray[i] = (char *) malloc(sizeof(char) * STR_LEN);
        sprintf(theArray[i], "String %d: the initial value", i);
	}


    FILE *f = fopen("mutex_10000.txt", "w");

	// set up the socket
	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	int clientFileDescriptor;
	pthread_t t[X]; // create 1000 threads

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=port_num;
	sock_var.sin_family=AF_INET;

	// socket creation success
	if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
		// listen to the socket
		listen(serverFileDescriptor,2000);
		while(1)        //loop infinity
		{
			// total time used in all threads
			double total = 0;

			for(i=0;i<X;i++)      //can support X clients at a time
			{
				// accept the connection request from client
				clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);

				// create a thread to handle a connection and pass required info to the thread
				struct FD_Thread *fd_thread = (struct FD_Thread*)malloc(sizeof(struct FD_Thread));
				fd_thread->fd = clientFileDescriptor;
				fd_thread->thread = i;
				pthread_create(&t[i],NULL,ServerEcho,(void *)fd_thread);
			}

			// join all threads and add up time
			for (i = 0; i < X; i++) {
				pthread_join(t[i],NULL);
				total += times[i];
			}

			printf("Total time: %f\n", total);
            f = fopen("mutex_10000.txt", "a");
            fprintf(f, "%f\n", total);
            fclose(f);
		}

		// close server
		close(serverFileDescriptor);
	}
	// socket creation failed
	else{
		printf("socket creation failed\n");
	}
	return 0;
}
Contact GitHub API Training Shop Blog About
© 2017 GitHub, Inc. Terms Privacy Security Status Help
