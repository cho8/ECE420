#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "sdk/timer.h"

#define STR_LEN 50		//max string length
#define X		1000	//number of threads

/* Global variables */
char** theArray;
int numstrings = 0;
pthread_mutex_t mutex;

void* Operate(void *args);

int main(int argc, char* argv[]) {
	int i;
	if (argc != 3) {
		printf("Usage: ./server <portnum> <numstrings>\n");
		return 0;
	}

	// Initialize theArray
	numstrings = atoi(argv[2]);
	theArray = (char **) malloc(sizeof(char *) * numstrings);


	for(i = 0; i < numstrings; i++) {
		theArray[i] = (char *) malloc(sizeof(char) * STR_LEN);
	  	sprintf(theArray[i], "String %d: the initial value", i);
	}

	// Start server
	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	int clientFileDescriptor;

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=atoi(argv[1]);
	sock_var.sin_family=AF_INET;

	pthread_t t[X];
	//pthread_mutex_init(&mutex, NULL);


	if (bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0) {
		printf("socket has been created\n");
		listen(serverFileDescriptor,2000);

		while(1) {    //loop infinity
			printf("New loop\n");
			for(i=0;i<X;i++) {      //can support X clients at a time

				clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
				//printf("Connected to client %d\n",clientFileDescriptor);
				pthread_create(&t[i], NULL, Operate, (void *)clientFileDescriptor);
			}

		}
		close(serverFileDescriptor);
	}
	else {
		printf("socket creation failed\n");
	}
	return 0;
}

void* Operate(void* args) {
	int clientFileDescriptor=(int)args;
	char str_clnt[STR_LEN];
	char str_ser[STR_LEN];
	char mode = '\0';
	int pos = 0;

	// Get pos from client
	read(clientFileDescriptor,str_clnt,STR_LEN);
	sscanf(str_clnt, "%c %d", &mode, &pos);

	if (mode == 'R') {
		pthread_mutex_lock(&mutex);
		strcpy(str_ser, theArray[pos]);
		pthread_mutex_unlock(&mutex);

	} else if (mode == 'W') {
		sprintf(str_ser, "String %d has been modifed by a write request", pos);
		pthread_mutex_lock(&mutex);
		strcpy(theArray[pos],str_ser);
		pthread_mutex_unlock(&mutex);

	} else {
		printf("Not a request\n");
		pthread_exit(0);
	}


	if (mode == 'R' || mode == 'W') {
		write(clientFileDescriptor,str_ser,STR_LEN);
	}
	close(clientFileDescriptor);
	pthread_exit(0);
}
