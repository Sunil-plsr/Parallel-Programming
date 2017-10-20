#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]) {
   int  a, b, n, intensity;
   int threads;

   /*validating command line arguments*/
  if (argc < 4)
  {
      printf("Please Input atleast 3 Command Line arguments\n");
      exit(0);
  }
  a = 1;
  b = 100;

        int number_of_threads, choice, dynamic_length;

        /*intializing command line arguments*/
        number_of_threads = atoi(argv[1]);
        dynamic_length = atoi(argv[2]);
        n = atoi(argv[3]);
        intensity = atoi(argv[4]);


omp_set_num_threads(number_of_threads);
  struct timeval start, end;
gettimeofday(&start, NULL);
double t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);

#pragma omp parallel
   {
    double intermediate_result, ans = 0;

      /*loop to caluclate numerical integration*/
      #pragma omp for schedule(dynamic, dynamic_length)
     for (int i = 0; i < n; ++i)
     {

       /*Caluclating differential equivalent*/
      intermediate_result = (a+(i+0.5)*((b-a)/n));
      /*function = x^2*/
        intermediate_result = intermediate_result * intermediate_result;
        intermediate_result = intermediate_result*((b-a)/n);
        ans += intermediate_result;

        /* Running specified intensity passed by argv[4]*/
        for (int k = 0; k < intensity; ++k)
              {
                 intermediate_result = intermediate_result * intermediate_result;
                 intermediate_result = sqrt(intermediate_result);
              }
      }
    

   }


   /*printf("FInal Answer is: %lf", ans);*/
    gettimeofday(&end, NULL);
double t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
double time_spent = t2-t1;



                printf("\n Threads:%d n:%d intensity:%d  dynamic_length:%d Time:%f",number_of_threads,n,intensity,dynamic_length, time_spent);
        


return EXIT_SUCCESS;
}
