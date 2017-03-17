#include <stdio.h>
#include <omp.h>
int main() {
	omp_set_nested(1);
	omp_set_dynamic(0);
 	#pragma omp parallel num_threads(2)
 	{
 		if (omp_get_thread_num() == 0)
 			omp_set_num_threads(4);
 		else
 			omp_set_num_threads(6);
 		printf("%d: %d %d\n", omp_get_thread_num(),
 			omp_get_num_threads(),
 			omp_get_max_threads());
 
		#pragma omp parallel
 		{
 			#pragma omp master
 			{
 				printf("Inner: %d\n", omp_get_num_threads());
 			}
 		}

 		omp_set_num_threads(7);

 		#pragma omp parallel
		{
 			printf("count me %d %d.\n", omp_get_thread_num(), omp_get_num_threads());
 		}
 	}
 	return(0);
}
