#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

int a[100000000];
/*Initializing with default values*/
int key = -143, granularity = 1000000, find_first_array[20] = {99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999,99999999};

/*
Initializing find_first_array to 999999999 (max value) so that we can check and store the lower index where we found number
thread_find_first function will find minimum withing the range of thread
*/

void initialize_array(int n)
{
    #pragma omp parallel for   
    for (int i=0; i<n; ++i)
        a[i] = rand()%1000;
}

void check_cmd_arguments(int arguments, int argc)
{
        if (arguments + 1 < argc)
        {
                printf("Please input sufficient number of command line arguments\n");
                exit(0);
        }
}

void thread_find_first(int index)
{
        for (int i = index; i < index+granularity; ++i)
        {
                if ((a[i] == key) && (find_first_array[omp_get_thread_num()] > i))
                {
                        find_first_array[omp_get_thread_num()] = i;
                }
        }
/*
        if(a[find_first_array[omp_get_thread_num()]] > a[thread_minimum_index])
                find_first_array[omp_get_thread_num()] = thread_minimum_index;
*/
}
void find_first(int n)
{
        for (int i = 0; i < n; i = i + granularity)
        {
                #pragma omp task
                thread_find_first(i);
        }

        #pragma omp taskwait

        int least_index = 99999999;
        /*Comparing which thread has least index value*/
        for (int i = 0; i < omp_get_num_threads(); ++i){
                if(find_first_array[i] < least_index)
                        least_index = find_first_array[i];
        }



        printf("element %d found in the list at %d:\n", a[least_index], least_index);

}


int main(int argc, char const *argv[])
{

    int n = 100000000,  number_of_threads;

    check_cmd_arguments(2, argc);

    number_of_threads = atoi(argv[1]);
    granularity = 1000000;

    initialize_array(n);

    a[256] = -143;
    omp_set_num_threads(number_of_threads);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    double t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);


    #pragma omp parallel
            {
                #pragma omp single
                {
                        find_first(n);
                }
            }

    gettimeofday(&end, NULL);
    double t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
    double time_spent = t2-t1;

    printf("\n Threads: %d,  granularity: %d, Time: %f",number_of_threads, granularity, time_spent);


    return 0;
}
