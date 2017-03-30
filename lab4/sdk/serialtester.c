/*
    Test the result stored in "data_output" against a serial implementation.

    -----
    Compiling:
    Include "Lab4_IO.c" to compile. Set the macro "LAB4_EXTEND" defined in the "Lab4_IO.c" file to include the extended functions
    $ gcc serialtester.c Lab4_IO.c -o serialtester -lm

    -----
    Return values:
    0      result is correct
    1      result is wrong
    2      problem size does not match
    253    no "data_output" file
    254    no "data_input" file
*/
#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab4_IO.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85

#define THRESHOLD 0.0001

int main (int argc, char* argv[]){
    struct node *nodehead;
    int nodecount;
    int *num_in_links, *num_out_links;
    double *r, *r_pre;
    int i, j;
    double damp_const;
    int iterationcount = 0;
    int collected_nodecount;
    double *collected_r;
    double cst_addapted_threshold;
    double error;
    FILE *fp;

    // Load the data and simple verification
    if ((fp = fopen("data_output", "r")) == NULL ){
    	printf("Error loading the data_output.\n");
        return 253;
    }
    fscanf(fp, "%d\n%lf\n", &collected_nodecount, &error);
    if (get_node_stat(&nodecount, &num_in_links, &num_out_links)) return 254;
    if (nodecount != collected_nodecount){
        printf("Problem size does not match!\n");
        free(num_in_links); free(num_out_links);
        return 2;
    }
    collected_r = malloc(collected_nodecount * sizeof(double));
    for ( i = 0; i < collected_nodecount; ++i)
        fscanf(fp, "%lf\n", &collected_r[i]);
    fclose(fp);

    // Adjust the threshold according to the problem size
    cst_addapted_threshold = THRESHOLD;

    // Calculate the result
    if (node_init(&nodehead, num_in_links, num_out_links, 0, nodecount)) return 254;

    r = malloc(nodecount * sizeof(double));
    r_pre = malloc(nodecount * sizeof(double));
    for ( i = 0; i < nodecount; ++i)
        r[i] = 1.0 / nodecount;
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;
    // CORE CALCULATION
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
    //printf("Program converges at %d th iteration.\n", iterationcount);

    // post processing
    node_destroy(nodehead, nodecount);
    free(num_in_links); free(num_out_links);

    // Compare the result
    error = rel_error(collected_r, r, nodecount);
    printf ("The relative error against the reference result is %e .\n", error);
    free(r); free(r_pre); free(collected_r);
    if ( error < cst_addapted_threshold){
        printf("Congratulations! Your result is correct!\n");
        return 0;
    }else{
        printf("Sorry. Your result is wrong.\n");
        return 1;
    }
}
