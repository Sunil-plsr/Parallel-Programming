#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[]) {
   int ans = 0, a, b, n, intensity;
   int threads;
  if (argc < 5)
  {
      printf("Please Input atleast 4 Command Line arguments\n");
      exit(0);
  }
  a = atoi(argv[1]);
  b = atoi(argv[2]);
  n = atoi(argv[3]);
  intensity = atoi(argv[4]);
   double intermediate_result;


   clock_t begin = clock();
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

   /*printf("FInal Answer is: %lf", ans);*/
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\n The time taken for integrating %d points with Operation Intensity %d is:%f",n, intensity, time_spent);

return EXIT_SUCCESS;
}