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
#include "rwlock.h"

#define STR_LEN 50		//max string length
#define X		1000	//number of threads


/* Global variables */
char** theArray;
int numstrings;
pthread_mutex_t mutex;
mylib_rwlock_t *rwlock;

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
	rwlock = (mylib_rwlock_t*) malloc (sizeof(mylib_rwlock_t) * numstrings);

	for(i = 0; i < numstrings; i++) {
		theArray[i] = (char *) malloc(sizeof(char) * STR_LEN);
	  	sprintf(theArray[i], "String %d: the initial value", i);
		mylib_rwlock_init(&rwlock[i]);
	}
	//mylib_rwlock_init(&rwlock);

	// Start server
	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	int clientFileDescriptor;

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=atoi(argv[1]);
	sock_var.sin_family=AF_INET;

	pthread_t t[X];
	pthread_mutex_init(&mutex, NULL);

	if (bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0) {
		printf("socket has been created\n");
		listen(serverFileDescriptor,2000);

		while(1) {    //loop infinity
			for(i=0;i<X;i++) {      //can support X clients at a time

				clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
				//printf("Connected to client %d\n",clientFileDescriptor);
				pthread_create(&t[i], NULL, Operate, (void *)clientFileDescriptor);
			}
			for(i=0;i<X;i++) {
				pthread_join(t[i],NULL);
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

	if (mode == 'W') {
		//printf("%d Want to write %d\n", clientFileDescriptor, pos);
		sprintf(str_ser, "String %d has been modifed by a write request", pos);

		mylib_rwlock_wlock(&rwlock[pos]);		
		sprintf(theArray[pos], str_ser);
		mylib_rwlock_unlock(&rwlock[pos]);

		//printf("%d Done writing %d\n", clientFileDescriptor, pos);
	} else if (mode == 'R') {
		//printf("%d Want to read %d\n", clientFileDescriptor, pos);
		mylib_rwlock_rlock(&rwlock[pos]);
		sprintf(str_ser, theArray[pos]);
		mylib_rwlock_unlock(&rwlock[pos]);		

		//printf("%d Done reading %d\n", clientFileDescriptor, pos);
	}

	write(clientFileDescriptor,str_ser,STR_LEN);
	close(clientFileDescriptor);
}
