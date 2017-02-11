#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define STR_LEN 20
#define X		2

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

	pthread_t t[20];
	pthread_mutex_init(&mutex, NULL);

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=atoi(argv[1]);
	sock_var.sin_family=AF_INET;

	if (bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0) {
		printf("socket has been created\n");
		listen(serverFileDescriptor,2000);

		while(1) {    //loop infinity
			for(i=0;i<X;i++) {      //can support X clients at a time

				clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
				printf("Connected to client %d\n",clientFileDescriptor);
				pthread_create(&t, NULL, Operate, (void *)clientFileDescriptor);
			}

			for(i=0;i<X;i++){		// join the threads
				pthread_join(t[i], NULL);
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
	printf("Reading from client %d: %c %d\n",clientFileDescriptor,mode,pos);

	// pthread_mutex_lock(&mutex);

	if (mode == 'R') {
		printf("Client reading\n");
		sprintf(str_ser, theArray[pos]);
	} else if (mode == 'W') {
		printf("client writing\n");
		sprintf(theArray[pos], "String %d has been modifed by a write request", pos);
		sprintf(str_ser, theArray[pos]);
	}
	// pthread_mutex_unlock(&mutex);


	if (mode == 'R' || mode == 'W') {
		printf("Echo to client\n");
		write(clientFileDescriptor,str_ser,STR_LEN);
	}
	close(clientFileDescriptor);
}
