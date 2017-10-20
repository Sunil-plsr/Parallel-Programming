/* Iterative C program for merge sort */
#include<stdlib.h>
#include<stdio.h>
#include<omp.h>
#include <math.h>
#include <sys/time.h>


int n = 1000000000;

int min(int x, int y) { return (x<y)? x :y; }




int main(int argc, char *argv[])
{


	int number_of_threads = atoi(argv[1]);
	int *arr;
	arr = (int *)malloc(sizeof(int)*n); 
	
    for (int i=0; i<n; ++i)
        arr[i] = rand()%1000;



		omp_set_num_threads(number_of_threads);

	struct timeval start, end;
gettimeofday(&start, NULL);
double t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);






int curr_size; 
				
int left_start;
				

for (curr_size=1; curr_size<=n-1; curr_size = 2*curr_size)
{
	#pragma omp parallel for
	for (left_start=0; left_start<n-1; left_start += 2*curr_size)
	{
		/*int mid = left_start + curr_size - 1;*/
		int mid = min(left_start + curr_size - 1, n - 1);

		int right_end = min(left_start + 2*curr_size - 1, n-1);

		
		int l = left_start, m = mid, r = right_end;


			int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;

	/* create temp arrays */
	
	int *L, *R;

	L = (int *)malloc(sizeof(int)*n1);
	R = (int *)malloc(sizeof(int)*n2);

	/* Copy data to temp arrays L[] and R[] */
	#pragma omp parallel for
	for (i = 0; i < n1; i++)
		L[i] = arr[l + i];
	#pragma omp parallel for
	for (j = 0; j < n2; j++)
		R[j] = arr[m + 1+ j];

	/* Merge the temp arrays back into arr[l..r]*/
	i = 0;
	j = 0;
	k = l;
	while (i < n1 && j < n2)
	{
		if (L[i] <= R[j])
		{
			arr[k] = L[i];
			i++;
		}
		else
		{
			arr[k] = R[j];
			j++;
		}
		k++;
	}
	

	/* Copy the remaining elements of L[], if there are any */
	while (i < n1)
	{
		arr[k] = L[i];
		i++;
		k++;
	}
	/* Copy the remaining elements of R[], if there are any */
	while (j < n2)
	{
		arr[k] = R[j];
		j++;
		k++;
	}

}
free(L);
free(R);
}






gettimeofday(&end, NULL);
double t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
double time_spent = t2-t1;
    printf("\n Threads: %d :: Time: %f",number_of_threads, time_spent);
	free(arr);
	return 0;
}