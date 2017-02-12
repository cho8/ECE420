#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define STR_LEN 50
#define X	100

/* Global Variables */
int port;
int threads;
int numstrings;
int *seed;

/* Thread function */
void* Operate(void* rank);


int main(int argc, char* argv[]) {

	if (argc != 3) {
		printf("Usage: ./client <portnum> <numstrings>\n");
		return 0;
	}

	int i, t;
	pthread_t thread_handles[X];

	/* Get command line args */
	port = atoi(argv[1]);
	numstrings = atoi(argv[2]);
	threads = X;

	/* Initialize random number generators */
	seed = malloc(X*sizeof(int));
	for (i=0; i< X; i++) {
		seed[i]=i;
	}

	/* Launch threads */
	for (t = 0; t < threads; t++) {
		pthread_create(&thread_handles[t], NULL, Operate, (void *) t);
	}

	for (t=0; t < threads; t++) {
		pthread_join(thread_handles[t], NULL);
	}


	return 0;
}

/* For each thread, launch a read/write request to the server */

void* Operate(void* rank) {
	long my_rank = (long) rank;
//	printf("Thread %ld \n", my_rank);
	char str_clnt[STR_LEN];
	char str_ser[STR_LEN];

	// Dispatch request via socket stream
	struct sockaddr_in sock_var;
	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=port;
	sock_var.sin_family=AF_INET;

	if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0) {
		//printf("Connected to server %d\n",clientFileDescriptor);

		// Find a random position in theArray for read or write
		int pos = rand_r(&seed[my_rank]) % numstrings;
		int randNum = rand_r(&seed[my_rank]) % 100;

		if (randNum >= 95) {// 5% are write operations, others are reads
			// write message
			sprintf(str_clnt, "W %d", pos);
			write(clientFileDescriptor,str_clnt, STR_LEN);

		} else {
			// read message
			sprintf(str_clnt, "R %d", pos);
			write(clientFileDescriptor,str_clnt, STR_LEN);
		}
		// else just read the message
		read(clientFileDescriptor,str_ser,STR_LEN);
		//printf("%s\n", str_ser);


		close(clientFileDescriptor);
	}
	else{
		printf("socket creation failed\n");
	}



	return NULL;
}
