#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lab1_IO.h"

/*
	Usage ./main thread_count
*/



/* Global variables */
int n;	// m,n of matrix (restricted to square matrices)
int thread_count;
int **A;	// int array pointer to input matrix A
int **B;	// int array pointer to input matrix B
int **C;	// int array pointer to result matrix
double time;	// time measurement

/* Serial function */

/* Parallel function */
void *matrix_mul(void* rank);

/*** Main ***/
int main(int argc, char* argv[]) {

	// Local variables
	long       thread;
	pthread_t* thread_handles;


	// TODO get dimensions (from either command line or otherwise)
	if (argc !=2 ) {
		printf("Usage: ./%s <threads>\n", argv[0]);
	}
	thread_count = atoi(argv[1]);


	// TODO load in matrices using lab1_IO.c
	// memory is allocated automatically
	Lab1_loadinput(&A, &B, &n);

	// TODO matrix multiplication serial (i.e. 1 thread)


	// TODO matrix multiplication parallel (and time)
	// create threads and perform matrix_mul
	// join threads

	// TODO save matrix using lab1_IO.c
	Lab1_saveoutput(C, n, time); // NOTE: output matrix arg might be weird here

	// TODO free matrix mallocs



	return 0;
}

/*
	Parallel matrix multiplication
	Multiple nxn matrix A by nxn matrix B
	Args: rank
*/
