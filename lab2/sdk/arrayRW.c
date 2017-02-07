/* File:  
 *    arrayRW.c
 *
 * Purpose:
 *    Illustrate multithreaded reads and writes to a shared array
 *
 * Input:
 *    none
 * Output:
 *    message from each thread
 *
 * Usage:    ./arrayRW <thread_count>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <string.h>
#include "timer.h"
#define NUM_STR 1024
#define STR_LEN 1000

int thread_count;  
char theArray[NUM_STR][STR_LEN];
int* seed;
pthread_mutex_t mutex;

void *Operate(void* rank);  /* Thread function */



/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	long       thread;  /* Use long in case of a 64-bit system */
	pthread_t* thread_handles; 
	int i;
	double start, finish, elapsed;	

	/* Get number of threads from command line */
	thread_count = strtol(argv[1], NULL, 10);  
	
	/* Intializes random number generators */
	seed = malloc(thread_count*sizeof(int));
	for (i = 0; i < thread_count; i++)
		seed[i] = i;
	
	/* Fill in the initial values for theArray */
	for (i = 0; i < NUM_STR; i ++)
	{
		sprintf(theArray[i], "theArray[%d]: initial value", i);
		printf("%s\n\n", theArray[i]);
	}
   
	thread_handles = malloc (thread_count*sizeof(pthread_t)); 
	pthread_mutex_init(&mutex, NULL);
	
	GET_TIME(start); 
	for (thread = 0; thread < thread_count; thread++)  
		pthread_create(&thread_handles[thread], NULL, Operate, (void*) thread);  

	for (thread = 0; thread < thread_count; thread++) 
		pthread_join(thread_handles[thread], NULL); 
	GET_TIME(finish);
	elapsed = finish - start;	
 	printf("The elapsed time is %e seconds\n", elapsed);
   
	pthread_mutex_destroy(&mutex);
	free(thread_handles);
	return 0;
}  /* main */


/*-------------------------------------------------------------------*/
void *Operate(void* rank) {
	long my_rank = (long) rank;
	
	// Find a random position in theArray for read or write
	int pos = rand_r(&seed[my_rank]) % NUM_STR;
	int randNum = rand_r(&seed[my_rank]) % 10;	// write with 10% probability
	
	pthread_mutex_lock(&mutex); 
	if (randNum >= 9) // 10% are write operations, others are reads
		sprintf(theArray[pos], "theArray[%d] modified by thread %d", pos, my_rank);
	printf("Thread %d: randNum = %d\n", my_rank, randNum);
	printf("%s\n\n", theArray[pos]); // return the value read or written
	pthread_mutex_unlock(&mutex);
		
	return NULL;
}