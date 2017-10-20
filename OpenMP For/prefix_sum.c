#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

int a[1000000000];

int main (int argc, char *argv[])
{  
        int size = 1000000000, number_of_threads;
        #pragma omp parallel for   
        for (int i=1; i<=size; ++i)
          {//a[i] = rand()%2;
           a[i]=1;}

/* validating command line arguments*/
  if (argc < 2)
  {
      printf("Please Input Command Line argument\n");
      exit(0);
  }

/*initializing command line argument*/
          number_of_threads = atoi(argv[1]);

        omp_set_num_threads(number_of_threads);
        int val_pos[100] = {0};
struct timeval start, end;
gettimeofday(&start, NULL);
double t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);
        #pragma omp parallel
                {
                        int thread_num = omp_get_thread_num();
                        printf("Thread number is: %d\n",thread_num);
                        /*doing prefix sum for dividing array into subarray's of sizes 50000000*/
                        #pragma omp for schedule(dynamic, 50000000)
                        for (int i = 2;i <= size; ++i)
                        {
                                a[i] = a[i] + a[i-1];
                        }

                        
                }

                /*Caluclating final prefix sum for whole array*/
        for (int i = 50000000; i <= size; i=i+50000000)
                        {
                                #pragma omp parallel for schedule(dynamic)
                                for (int j = i+1;j <= i+50000000-1; ++j)
                                {
                                        a[j] = a[j] + a[i];
                                }
                        }
gettimeofday(&end, NULL);
double t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
double time_spent = t2-t1;
       printf("\n Threads: %d :: Time: %f",number_of_threads, time_spent);
        return 0;
}