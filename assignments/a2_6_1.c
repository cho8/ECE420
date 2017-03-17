#include <stdlib.h>
#include <stdio.h>
#include <math.h>


int fib_tasks(int n) {
	int i, j;
	if (n<2)
		return n;
	else {
		#pragma omp task shared(i) firstprivate(n)
		i = fib_tasks(n-1);
		#pragma omp task shared(j) firstprivate(n)
		j = fib_tasks(n-2);
		#pragma omp taskwait
		return i+j;
	}
}

int main(int argc, char *argv[]) {
	int result;
	printf("Doing sections fibonacci:\n");
	result = fib_tasks(7);
	printf("Result is %d\n", result);
	return 0;
}

