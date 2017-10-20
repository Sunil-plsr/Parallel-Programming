#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

int a[100000000];
/*initializing with default values*/
int granularity = 1000, min_array[20] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

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

void thread_find_min(int index)
{
        int thread_minimum_index = index;
        for (int i = index; i < index+granularity; ++i)
        {
                if (a[i] < a[thread_minimum_index])
                {
                        thread_minimum_index = i;
                }
        }
        /*Computing thread minimum*/
        if(a[min_array[omp_get_thread_num()]] > a[thread_minimum_index])
                min_array[omp_get_thread_num()] = thread_minimum_index;

}
void find_min(int n)
{
        for (int i = 0; i < n; i = i + granularity)
        {
                #pragma omp task
                thread_find_min(i);
        }

        #pragma omp taskwait

        int global_min_index = 1;

        for (int i = 0; i < omp_get_num_threads(); ++i){
                if(a[min_array[i]] < a[global_min_index])
                        global_min_index = min_array[i];
        }



}


int main(int argc, char const *argv[])
{

    int n = 100000000,  number_of_threads;

    check_cmd_arguments(2, argc);

    number_of_threads = atoi(argv[1]);
    granularity = atoi(argv[2]);

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
                        find_min(n);
                }
            }

    gettimeofday(&end, NULL);
    double t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
    double time_spent = t2-t1;

    printf("\n Threads: %d,  granularity: %d, Time: %f",number_of_threads, granularity, time_spent);


    return 0;
}
