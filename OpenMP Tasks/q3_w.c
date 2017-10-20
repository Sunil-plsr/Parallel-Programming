/* Iterative C program for merge sort */
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <sys/time.h>

int arr[1000000000];


/* Function to merge the two haves arr[l..m] and arr[m+1..r] of array arr[] */
void merge(int arr[], int l, int m, int r);

int min(int x, int y) { return (x<y)? x :y; }

void check_cmd_arguments(int arguments, int argc)
{
        if (arguments + 1 < argc)
        {
                printf("Please input sufficient number of command line arguments\n");
                exit(0);
        }
}

/* Iterative mergesort function to sort arr[0...n-1] */
void mergeSort(int arr[], int start, int n)
{
int curr_size;
int left_start;

    for (curr_size=1; curr_size<=n-1; curr_size = 2*curr_size)
    {


        for (left_start=start; left_start<n-1; left_start += 2*curr_size)
        {
                /*int mid = left_start + curr_size - 1;*/
                int mid = min(left_start + curr_size - 1, n - 1);
                int right_end = min(left_start + 2*curr_size - 1, n-1);
                merge(arr, left_start, mid, right_end);
        }
    }


}
/* Function to merge the two haves arr[l..m] and arr[m+1..r] of array arr[] */
void merge(int arr[], int l, int m, int r)
{

        int i, j, k;
        int n1 = m - l + 1;
        int n2 = r - m;

        /* create temp arrays */
        int *L, *R;

        L = (int *)malloc(sizeof(int)*n1);
        R = (int *)malloc(sizeof(int)*n2);

        /* Copy data to temp arrays L[] and R[] */
        for (i = 0; i < n1; i++)
                L[i] = arr[l + i];
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
free(L);
free(R);
}


/* Driver program to test above functions */
int main(int argc, char const *argv[])
{

    int number_of_threads;

   check_cmd_arguments(2, argc);
 
    number_of_threads = atoi(argv[1]);


    omp_set_num_threads(number_of_threads);

    int n = 1000000000, granularity = 5000000;

    #pragma omp parallel for
    for (int i=0; i<n; ++i)
        arr[i] = rand()%1000;

    struct timeval start, end;
    gettimeofday(&start, NULL);
    double t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);



    #pragma omp parallel
       {
           #pragma omp single
           {
            for (int i = 0; i < n; i=i+granularity)
            {
            	/*calling omp task with array bounds to execute*/
                #pragma omp task
               mergeSort(arr, i, i+granularity);
            }

           }
       }

    #pragma omp taskwait

    gettimeofday(&end, NULL);
    double t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
    double time_spent = t2-t1;
    printf("\n Threads: %d :: Time: %f",number_of_threads, time_spent);
        return 0;
}
