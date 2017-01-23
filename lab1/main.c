#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "sdk/lab1_IO.h"
#include "sdk/timer.h"

/*
	Usage ./main thread_count
*/



/* Global variables */
int n;	// m,n of matrix (restricted to square matrices)
int thread_count;
int **A;	// int array pointer to input matrix A
int **B;	// int array pointer to input matrix B
int **C;	// int array pointer to result matrix

/* Serial function */

/* Parallel function */
void* matrix_mul(void* rank);
void print_matrix(int** m);

/*** Main ***/
int main(int argc, char* argv[]) {

	// Local variables
	long thread;
	pthread_t* thread_handles;
	double start_time;
	double end_time;


	// get dimensions (from either command line or otherwise)
	if (argc !=2 ) {
		printf("Usage: ./%s <threads>\n", argv[0]);
		return 0;
	}

	thread_count = atoi(argv[1]);
	thread_handles = malloc(thread_count*sizeof(pthread_t));


	// load in matrices using lab1_IO.c
	// allocate mem for matrix C
	Lab1_loadinput(&A, &B, &n);

	C = (int**) malloc(n*sizeof(int**));
	int i;
	for (i=0; i<n; i++) {
		C[i] = (int*) malloc (n*sizeof(int*));
	}


	// matrix multiplication parallel (and time)
	// create threads and perform matrix_mul
	GET_TIME(start_time);
	for (thread = 0; thread < thread_count; thread++)
	   pthread_create(&thread_handles[thread], NULL,
		  matrix_mul, (void*) thread);
	// join threads
	for (thread = 0; thread < thread_count; thread++)
       pthread_join(thread_handles[thread], NULL);

	GET_TIME(end_time);

	// save matrix using lab1_IO.c
	Lab1_saveoutput(C, &n, end_time-start_time); // NOTE: output matrix arg might be weird here

	// free matrix mallocs
	free(A);
	free(B);
	free(C);

	return 0;
}

/*
	Parallel matrix multiplication
	Multiple nxn matrix A by nxn matrix B by allowing a thread to calculate a
	block of elements in the result matrix.
	Block division scheme described in Lab1 manual.
	Variables used also correspond to the ones defined in the manual.
	Args: rank -- thread rank
*/
void* matrix_mul(void* rank) {
	long k = (long)rank;		// my_rank of thread
	int p = thread_count;

	// Calculate xy of block
	int x = k/sqrt(p);
	int y = (int)k % (int)sqrt(p);
	int i,j,m;

	// Calculate upper and lower bounds for x and y of block
	int lower_x = (n/sqrt(p))*x;
	int upper_x = (n/sqrt(p))*(x+1) - 1;
	int lower_y = (n/sqrt(p))*y;
	int upper_y = (n/sqrt(p))*(y+1) - 1;

	// for each element in the block
	for (i=lower_x; i<=upper_x;i++) {
		for(j=lower_y; j<=upper_y;j++) {

			// do colxrow multiplication for the element
			for(m=0; m<n; m++) {
				C[i][j] += A[i][m] * B[m][j];
			}
		}
	}
	return NULL;
}

/*
	Print matrix for funsies
*/
void print_matrix(int** C) {
	int i,j;
	for (i=0; i<n; i++) {
		printf("\n");
		for(j=0; j<n;j++) {
			printf("%d ", C[i][j]);
		}
	}
}
