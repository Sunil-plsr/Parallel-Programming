#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/time.h>

int a[100000000];

int main (int argc, char *argv[])
{
        int size = 100000000,  number_of_threads, find_val = 77;

/*validating command line arguments*/
          if (argc < 2)
  {
      printf("Please Input Command Line argument\n");
      exit(0);
  }

  /*initializing command line arguments*/
        number_of_threads = atoi(argv[1])

/*initializing array*/
#pragma omp for   
for (int i=0; i<size; ++i)
        a[i] = rand()%1000000;



int val_pos[100] = {0};
omp_set_num_threads(number_of_threads);
struct timeval start, end;
gettimeofday(&start, NULL);
double t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);
#pragma omp parallel
                {
                        int thread_num = omp_get_thread_num();
                        printf("Thread number is: %d\n",thread_num);
                        #pragma omp for
                        for (int i = 0;i < size; ++i)
                        {
                                if (a[i] == find_val)
                                {
                                        val_pos[thread_num] = i;
                                        
                                        i = size+1;
                                }
                        }
                }
    
int least_pos = 99999999;
for (int i = 0; i < number_of_threads; ++i)
        if (val_pos[i] < least_pos)
                least_pos = val_pos[i]; 
gettimeofday(&end, NULL);
double t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
double time_spent = t2-t1;
       printf("\n Threads: %d :: Time: %f",number_of_threads, time_spent);
}
                                                                                    