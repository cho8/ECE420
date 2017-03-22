#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "sdk/Lab3IO.h"
#include "sdk/timer.h"

int threads;
int size;
double **A;

/* Prototypes */
void swap_rows( int src, int dest);
int find_max(int k);
int savePrintedOutput( int size, double time, char * name);

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: ./main <num_threads>\n");
		return 0;
	}

	threads = atoi(argv[1]);

	// Load matrix and vector from Lab3IO
	Lab3LoadInput(&A, &size);
	//savePrintedOutput(size,10,"Input.txt");
	double result[size];

	//  Start time measurement
	double start_time=0;
	double end_time=0;
	GET_TIME(start_time);

	// Launch threads
	int k;
	#pragma omp parallel num_threads(threads) \
		default(none) shared(A,size) private(k)
	{
		/* Gaussian Elimination */
		for (k=0; k < size; k++) {

			#pragma omp single
			{
				// Pivoting
				// Find index of max
				int max_index = find_max(k);

				// swap rows
				swap_rows(k,max_index);
			}
			// Elimination
			int i;
			int j;

			#pragma omp for schedule(static, 8) private(i,j)
			for(i=k+1; i<size; i++) {
				double temp = (A[i][k]/A[k][k]);
				// printf("Coefficient %d , %f\n", i, temp);
				for (j=k; j<size+1; j++) {
					A[i][j] = A[i][j] - (temp* A[k][j]);
				}
			}


		}

		// Wait for all threads to finish Gaussian
		#pragma omp barrier

		// print the intermediate step for debugging
		// #pragma omp single
		// savePrintedOutput(A, size, 10, "Gaussian_inter.txt");

		/* Jordan Elimination */
		int i;
		for(k=size-1; k>0; k--) {
			#pragma omp for schedule(static, 8) private(i)
			for(i=0; i<k; i++) {
				A[i][size] = A[i][size] - (A[i][k] / A[k][k])*A[k][size];
				A[i][k] = A[i][k] - ((A[i][k]/A[k][k])*A[k][k]);
			}
		}


	}
	printf("out of loop\n");

	// End time measurement
	GET_TIME(end_time);


	// Get output and save it

	int i;
	for (i = 0; i < size; i++){
		//printf("index %d\n", i);
		result[i] = A[i][size] / A[i][i];
		//printf("%d %f\n", i, result[i]);
	}

	//savePrintedOutput(size, end_time-start_time, "Result.txt");

	Lab3SaveOutput(result, size, end_time-start_time);
	printf("Total Time: %f \n",end_time-start_time);


	return 0;
}


/********
	In U, from row k to row n, find the row kp that has the maximum absolute
	value of the element in the kth column.
	args:
		k -- column index
	global:
		A
********/
int find_max(int k) {

	double curr_max = fabs(A[k][k]); // assuming square coefficient matrix
	int curr_max_index=k;
	int i;
	for (i = k; i<size; i++) {
		int value = fabs(A[i][k]);
		if (value > curr_max) {
			curr_max_index=i;
			curr_max = value;
		}
	}
	//return 0;
	return curr_max_index;
}


/********
	Swap the rows in the augmented matrix A;
	args:
		src, dest -- row indices to be swapped
	global:
		A
********/
void swap_rows(int src, int dest) {
	double * temp;
	temp = A[src];
	A[src] = A[dest];
	A[dest] = temp;
}


/********
	Print the intermediate matrix to file.
	args:
		size -- dimension of square matrix
		time -- running time
		name -- name of file
********/
int savePrintedOutput( int size, double time, char * name) {
	FILE* op;
	int i, j;

	if ((op = fopen(name,"w")) == NULL){
		printf("Error opening the output file.\n");
		return 1;
	}

	fprintf(op, "%d\n\n", size);
	fprintf(op, "%lf\n\n", time);
	for (i = 0; i < size; i++) {
		printf("%d ", i);
		for (j = 0; j< size+1; j++) {
			printf("%d %f\n", j, A[i][j]);
			fprintf(op, "%f\t", A[i][j]);
		}
		fprintf(op, "\n");
	}
	fclose(op);
	return 0;
}
