#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sdk/Lab4_IO.h"
#include "sdk/timer.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85

int main (int argc, char* argv[]){
    struct node *nodehead;
    int nodecount;
    int *num_in_links, *num_out_links;
    double *r, *r_pre, *r_local;
    int i, j;
    double damp_const;
    int iterationcount = 0;
    int collected_nodecount;
    double *collected_r;
    double cst_addapted_threshold;
    double error;

    double start_time, end_time;

    // Adjust the threshold according to the problem size
    cst_addapted_threshold = THRESHOLD;

    // Get data_input
    if (get_node_stat(&nodecount, &num_in_links, &num_out_links)) return 254;
    if (node_init(&nodehead, num_in_links, num_out_links, 0, nodecount)) return 254;

    r = malloc(nodecount * sizeof(double));
    r_pre = malloc(nodecount * sizeof(double));
    r_local = malloc(nodecount * sizeof(double));

    for ( i = 0; i < nodecount; ++i)
        r[i] = 1.0 / nodecount;
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;

    // CORE CALCULATION
    GET_TIME(start_time);
    do{
        ++iterationcount;
        vec_cp(r, r_pre, nodecount);
        for ( i = 0; i < nodecount; ++i){
            r[i] = 0;
            for ( j = 0; j < nodehead[i].num_in_links; ++j)
                r[i] += r_pre[nodehead[i].inlinks[j]] / num_out_links[nodehead[i].inlinks[j]];
            r[i] *= DAMPING_FACTOR;
            r[i] += damp_const;
        }
    }while(rel_error(r, r_pre, nodecount) >= EPSILON);
    GET_TIME(end_time);
    //printf("Program converges at %d th iteration.\n", iterationcount);
    printf("Time: %f\n", end_time-start_time);


    // post processing
    Lab4_saveoutput(r, nodecount, end_time-start_time);
    node_destroy(nodehead, nodecount);
    free(num_in_links); free(num_out_links);
    free(r); free(r_pre); free(collected_r); free(local_r);
    MPI_finalize();
}
