#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define LAB4_EXTEND

#include "sdk/Lab4_IO.h"
#include "sdk/timer.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85
#define THRESHOLD 0.0001


int main (int argc, char* argv[]) {
    if(argc != 1) {
        printf("Usage: ./main\n");
        return 0;
    }

    struct node *nodehead;
    int nodecount;
    int *num_in_links, *num_out_links;
    double *r, *r_pre, *r_local;
    int i, j;
    double damp_const;
    int iterationcount = 0;
    double start_time, end_time;
	
    int rank;
    int n;
    int chunksize;

    // Get MPI meta_data
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get data_input
    if (get_node_stat(&nodecount, &num_in_links, &num_out_links)) return 254;
    if (node_init(&nodehead, num_in_links, num_out_links, 0, nodecount)) return 254;

    r = malloc(nodecount * sizeof(double));
    r_pre = malloc(nodecount * sizeof(double));
 
    chunksize = nodecount / n;
    r_local = malloc(chunksize * sizeof(double));

    for ( i = 0; i < nodecount; ++i)
        r[i] = 1.0 / nodecount;
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;

    // Core calculation
    GET_TIME(start_time);
    do{
        ++iterationcount;
        vec_cp(r, r_pre, nodecount);
        for ( i = 0; i < chunksize; ++i) {
            r[i] = 0;
	    r_local[i]=0;
	    if ( i % n == rank) {
            	for (j = 0; j < nodehead[i].num_in_links; ++j) {
                	r_local[i] += r_pre[nodehead[i].inlinks[j]] / num_out_links[nodehead[i].inlinks[j]];
            	}
	    	r_local[i] *= DAMPING_FACTOR;
            	r_local[i] += damp_const;
	    }
        }
	// sum up result
	MPI_Allreduce(r_local, r, nodecount, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    } while(rel_error(r, r_pre, nodecount) >= EPSILON);
    GET_TIME(end_time);
    //printf("Program converges at %d th iteration.\n", iterationcount);

	
    // post processing
    Lab4_saveoutput(r, nodecount, end_time-start_time);
    node_destroy(nodehead, nodecount);
    free(num_in_links); 
    free(num_out_links);
    free(r); 
    free(r_pre); 
    free(r_local);
    MPI_Finalize();
    
    return 0;
}
