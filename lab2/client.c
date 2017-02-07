#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define X	1000

/* Global Variables */
int port:
int threads;
int *seed;

pthread_mutex_t mutex;

/* Thread function */
void *Operate(void* rank);


int main(int argc, char* argv[]) {

	if (argc != 3) {
		printf("Usage: ./client <portno> <threads>\n");
		return 0;
	}

	int i, t;
	pthread_t thread_handles[X];

	/* Get command line args */
	port = atoi(argv[1]);
	threads = atoi(argv[2]);

	/* Initialize random number generators */
	seed = malloc(X*sizeof(int));
	i
	for (i=0; i< X; i++) {
		seed[i]=i;
	}

	/* Launch threads */
	int t;
	for (t = 0; t < threads; t++) {
		pthread_create(&thread_handles[t], NULL, Operate, (void *) thread);
	}

	for (t=0; t < threads; t++) {
		pthread_join(thread_hanldes[thread], NULL);
	}


	return 0;
}

/* For each thread, launch a read/write request to the server */

void *Operate(void* rank) {
	long my_rank = (long) rank;
	printf("Thread %d \n", my_rank);

	// Dispatch request via socket stream
	struct sockaddr_in sock_var;
	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	char str_clnt[20],str_ser[20];

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;

	if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
		printf("Connected to server %dn",clientFileDescriptor);

		// Find a random position in theArray for read or write
		int pos = rand_r(&seed[my_rank]) % NUM_STR;
		int randNum = rand_r(&seed[my_rank]) % 10;	// write with 10% probability

		pthread_mutex_lock(&mutex);
		if (randNum >= 95) {// 5% are write operations, others are reads
			// write message
		} else {
			// read message
		}


		close(clientFileDescriptor);
	}
	else{
		printf("socket creation failed");
	}



	return NULL;
}