#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>
/*Caluclations are made using rank to determine start and end iteration on which every node works on,
after completion each node answers are sent to node 0, in which each node answers are summed to get the final answer*/

int main(int argc, char *argv[]) {
   int  a, b, n, intensity, recieve[32];
   int threads;
  a = 1;
  b = 100;
  n = atoi(argv[1]);
  intensity = atoi(argv[2]);

  int number_of_threads, choice, dynamic_length;
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);


    for (int i = 0; i < size; ++i)
    {
      recieve[i] = 0;
    }


    double t1, t2, time_spent;
    struct timeval start, end;
    if (rank == 0)
    {
      gettimeofday(&start, NULL);
      t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);
    }
    
    int range = n / size, start_iteration = (rank)*range, end_iteration = (rank+1)*range ;

    double ans = 0;

#pragma omp parallel
   {
    double intermediate_result;

     for (int i = start_iteration; i < end_iteration; ++i)
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


    if (rank != 0)
    {
        MPI_Send(&ans, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }


    double final_answer = 0;
   
    if (rank == 0)
    {
      for (int i = 1; i < size; ++i)
      {
        MPI_Recv(&recieve[i], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);      
        final_answer = final_answer + recieve[i];
      }
      gettimeofday(&end, NULL);
      t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
      time_spent = t2-t1;
    }
    

   /*printf("Node %d has executed from %d to %d iterations\n", rank, start_iteration, end_iteration );
*/

MPI_Finalize();
if (rank == 0) {
    printf("\n N: %d, operation_intensity: %d, Cores: %d  Time spent : %f", n, intensity, size, time_spent);
}
return EXIT_SUCCESS;
}