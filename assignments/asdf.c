#include <stdlib.h>
#include <stdio.h>
#include <math.h>


int fib_sections(int n) {
	int i, j, res;
	if (n<2)
		res= n;
	else {
			i = fib_sections(n-1);
			j = fib_sections(n-2);
		res= i+j;
	}
	printf("%d %d\n",n,res);
	return res;
}

int main(int argc, char *argv[]) {
	int result;
	printf("Doing sections fibonacci:\n");
	result = fib_sections(7);
	printf("Result is %d\n", result);
	return 0;
}

