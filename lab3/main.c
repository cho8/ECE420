#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "sdk/Lab3IO.h"
#include "sdk/timer.h"

int threads;
double **A;		// Augmented matrix [A|b]
int size;

/* Prototypes */
void gaussian_elim();
void jordan_elim();
void swap_rows(int src, int dest);
int find_max(int k);
int saveOutput(double **A, int size, double time, char * name);

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: ./main <num_threads>");
		return 0;
	}

	threads = atoi(argv[1]);
	double result[size];

	// Load matrix and vector from Lab3IO
	Lab3LoadInput(&A, &size);

	//  Start time measurement
	double start_time, end_time;
	GET_TIME(start_time);


	#pragma omp parallel num_threads(thread) {
	// Gaussian
	gaussian_elim();

	saveOutput(A, size, 10, "Gause_inter.txt");
	#pragma omp barrier

	// Jordan
	jordan_elim();
	saveOutput(A, size, 10, "Jordan_inter.txt");
	}

	// End time measurement
	GET_TIME(end_time);

	// Save Output
	int i;
	for (i = 0; i < size; i++){
		result[i] = A[i][size] / A[i][i];
	}
	Lab3SaveOutput(result, size, end_time-start_time);
	printf("Total Time: %f \n",end_time-start_time);


	return 0;
}

/********
	Gaussian Elimination will transform the augmented matrix into its equivalent
	upper triangular form. This is algorithm 1 in the lab manual.
	global:
		A
********/
void gaussian_elim() {
	int k;


	for (k=0; k < size; k++) {

		#pragma omp single {
			// Pivoting
			int max_index = find_max(k);
			swap_rows(k, max_index);
		}
		// Elimination
		int i;
		int j;

		#pragma omp for schedule(dynamic,1) private(i,j)
		for(i=k+1; i<size; i++) {
			double temp = (A[i][k]/A[k][k]);
			printf("%f\n", temp);
			for (j=k; j<size+1; j++) {
				A[i][j] = A[i][j] - (temp* A[k][j]);
			}
		}

	}

}

/*********
	Jordan Elimination further transforms the upper triangular form into the
	desired reduced form. This is algorithm 2 in the lab manual.
*********/
void jordan_elim() {
	int k;
	for(k=size-1; k>0; k--) {
		int i;
		#pragma omp for schedule(dynamic,1) private(i)
		for(i=0;i<k;i++) {
			A[i][size] = A[i][size] - (A[i][k] / A[k][k])*A[k][size];
			A[i][k] = A[i][k] - (A[i][k]/A[k][k])*A[k][k];
		}
	}
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
	int i;
	double curr_max = fabs(A[k][k]); // assuming square coefficient matrix
	int curr_max_index=k;
	for (i = 0; i<size; i++) {
		int value = fabs(A[i][k]);
		if (value > curr_max) {
			curr_max_index=i;
			curr_max = value;
		}
	}

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
	Print the intermediate matrix to file
********/
int saveOutput(double **A, int size, double time, char * name) {
	FILE* op;
	int i, j;

	if ((op = fopen(name,"w")) == NULL){
		printf("Error opening the output file.\n");
		return 1;
	}

	fprintf(op, "%d\n\n", size);
	fprintf(op, "%lf\n\n", time);
	for (i = 0; i < size; i++) {
		for (j = 0; j< size + 1; j++)
			fprintf(op, "%f\t", A[i][j]);
		fprintf(op, "\n");
	}
	fclose(op);
	return 0;
}
