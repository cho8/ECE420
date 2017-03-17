/* Program 2 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void* thread_function(void * arg) {
    int number = 10;
    int * b = &number;
    pthread_exit((void*)b);
}

int main() {
    pthread_t thread_id;
    int *status;

    pthread_create(&thread_id, NULL, &thread_function, NULL);

    pthread_join(thread_id, (void**)&status);

    printf("The worker thread has returned the status %d\n", *status);
    pthread_exit(NULL);
}

