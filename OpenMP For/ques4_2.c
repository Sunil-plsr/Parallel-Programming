#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/time.h>

int a[100000000];

int main (int argc, char *argv[])
{
        int size = 100000000, number_of_threads, find_val = 77;


/*checking command line arguments*/
  if (argc < 2)
  {
      printf("Please Input Command Line argument\n");
      exit(0);
  }
/*initializing command line argument*/0
  number_of_threads =  = atoi(argv[1]);

#pragma omp for   
for (int i=0; i<size; ++i)
        a[i] = rand()%1000000;


int val_pos = -1;
omp_set_num_threads(number_of_threads);
struct timeval start, end;
gettimeofday(&start, NULL);
double t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);


int flag = 0;
for (int i = 0; flag == 0; ++i)
{
        if ((i*(i+1)/2) >= 100000000)
        {
                flag = i;
        }
}



int k;

#pragma omp parallel
                {
                        int thread_num = omp_get_thread_num();
                        #pragma omp for schedule(dynamic, 1)
                        for (k = 0; k <= flag; ++k)
                        {
                                int cond;
                                if (k+k < size)
                                        cond = size;
                                else
                                        cond = k+k;

                                for (int i = k;i < cond; ++i)
                                {
                                        if (a[i] == find_val)
                                        {
                                                val_pos = i;
                                                i = cond+1;
                                                k = flag+1;
                                        }
                                }
                        }
                }


gettimeofday(&end, NULL);
double t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
double time_spent = t2-t1;
       printf("\n Threads: %d :: Time: %f",number_of_threads, time_spent);

}
