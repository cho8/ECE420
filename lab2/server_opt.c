#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "timer.h"

#define STR_LEN 50		//max string length
#define X		1000	//number of threads

/* typedef */
typedef struct {
	int fd; // file descriptor
	int id; // thread rank
} fd_thread_t;

/* Global variables */
char** theArray;
int numstrings = 0;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
double total_time;
double *times;
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
	times = (double *) malloc(sizeof(double) * X);

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
	pthread_rwlock_init(&rwlock, NULL);

	if (bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0) {
		printf("socket has been created\n");
		listen(serverFileDescriptor,2000);

		while(1) {    //loop infinity
			total_time =0 ;
			for(i=0;i<X;i++) {      //can support X clients at a time

				clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
				//printf("Connected to client %d\n",clientFileDescriptor);
				fd_thread_t * ft = (fd_thread_t *)malloc(sizeof(fd_thread_t));				
				ft->fd = clientFileDescriptor;
				ft->id = i;
				pthread_create(&t[i], NULL, Operate, (void *)ft);
			}
			for(i=0;i<X;i++) {
				pthread_join(t[i], NULL);;
				total_time += times[i];
			}
			printf("%f\n", total_time);
			FILE *f = fopen("rwl_1000.txt", "a");
			fprintf(f, "%f\n", total_time);
			fclose(f);
		}
		close(serverFileDescriptor);
	}
	else {
		printf("socket creation failed\n");
	}
	return 0;
}

void* Operate(void* args) {
	fd_thread_t * ft = (fd_thread_t*) args;
	int clientFileDescriptor=ft->fd;
	char str_clnt[STR_LEN];
	char str_ser[STR_LEN];
	double start_time, end_time;
	char mode = '\0';
	int pos = 0;


	// Get pos from client
	read(clientFileDescriptor,str_clnt,STR_LEN);
	sscanf(str_clnt, "%c %d", &mode, &pos);

	GET_TIME(start_time);
	
	if (mode == 'R') {
		pthread_rwlock_rdlock(&rwlock);
		strcpy(str_ser, theArray[pos]);
		pthread_rwlock_unlock(&rwlock);
	}else if (mode == 'W') {
		pthread_rwlock_wrlock(&rwlock);
		sprintf(str_ser, "String %d has been modifed by a write request", pos);
		strcpy(theArray[pos],str_ser);
		pthread_rwlock_unlock(&rwlock);
	}
	GET_TIME(end_time);

	write(clientFileDescriptor,str_ser,STR_LEN);
	close(clientFileDescriptor);
	times[ft->id] = end_time - start_time;
}
