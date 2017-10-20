#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/time.h>

int a[100000000]

int main (int argc, char *argv[])
{
        int n = 100000000, number_of_threads, choice, dynamic_length;
/*validating commmand line arguments*/
           if (argc < 4)
  {
      printf("Please Input atleast 3 Command Line arguments\n");
      exit(0);
  }
          /*intializing command line arguments*/

        number_of_threads = atoi(argv[1]);
        choice = atoi(argv[2]);

        if (choice == 2)
        {
                dynamic_length = atoi(argv[3]);
        }


/*initializing array*/
        #pragma omp parallel for   
        for (int i=0; i<n; ++i)
                a[i] = rand()%1000;


/*creating temporary value for minimum variable*/
        int min_val = a[0];
        int min_index = 0;




omp_set_num_threads(number_of_threads);
struct timeval start, end;
gettimeofday(&start, NULL);
double t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);
        if (choice == 1)
{       
        #pragma omp parallel for schedule(static) reduction(min:min_val)
        for(int i = 1; i<n; i++)
        {
            if(a[i]<min_val)
            {
                min_val = a[i];
                min_index = i;
                }
        }
}
else
{
        /*using dynamic*/
        #pragma omp parallel for schedule(dynamic, dynamic_length) reduction(min:min_val)
        for(int i = 1; i<n; i++)
        {
            if(a[i]<min_val)
            {
                min_val = a[i];
                min_index = i;
                }
        }

}
        gettimeofday(&end, NULL);
        double t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
        double time_spent = t2-t1;
        if (choice == 1)
        {
                printf("\n Threads: %d Schedule: Static Time: %f",number_of_threads, time_spent);
        }
        else
        {
                printf("\n Threads: %d Schedule: dynamic  dynamic_length: %d Time: %f",number_of_threads,dynamic_length, time_spent);
        }

        printf("\n The minimum value in an array of size 10^8 is %d and it is at index %d", min_val, min_index);

}
