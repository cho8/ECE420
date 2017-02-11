#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define STR_LEN 20
#define NUM_STR 1024

/* Global variables */
char theArray[NUM_STR][STR_LEN];
int numstrings = 0;
pthread_mutex_t mutex;

void* Operate(void *args);

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("Usage: ./server <portnum> <numstrings>\n");
		return 0;
	}

	numstrings = atoi(argv[2]);

	// Start server
	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	int clientFileDescriptor;
	int i;

	pthread_t t[20];
	pthread_mutex_init(&mutex, NULL);

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=atoi(argv[1]);
	sock_var.sin_family=AF_INET;

	if (bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0) {
		printf("socket has been created\n");
		listen(serverFileDescriptor,2000);

		while(1) {    //loop infinity
			for(i=0;i<20;i++) {      //can support 20 clients at a time

				clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
				printf("Connected to client %d\n",clientFileDescriptor);
				pthread_create(&t,NULL,Operate,(void *)clientFileDescriptor);
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
	char str[STR_LEN];
	int pos = 0;

	// Get pos from client
	read(clientFileDescriptor,str,STR_LEN);
	printf("Reading from client %d: %s\n",clientFileDescriptor,str);

	//

	// write(clientFileDescriptor,str,20);
	close(clientFileDescriptor);
}
