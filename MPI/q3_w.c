#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

/*
In this program node 0 acts as a master node, and every other node available in network acts as a worker node
master node will send to each node, from which they have to start performing their caluclations
Every node will compute from start_iteration value provided by node 0 to start_iteration+chunk
After computation is done, each node sends their answer to master(node 0), node 0 then summates all of the results and produces final answer
*/


int main(int argc, char *argv[]) {
   int  a, b, n, intensity, recieve_temp, recieve_ans = 0, current_start_send = 0;
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
  int chunk = n/40;



    double t1, t2, time_spent;
    struct timeval start, end;
    if (rank == 0)
    {
      gettimeofday(&start, NULL);
      t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);

      /*//sending initial work to every core*/
      for (int i = 1; i < size; ++i)
      {
          MPI_Send(&current_start_send, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
          current_start_send = current_start_send + chunk;
      }

      /*//sending additional work to completed processors*/
      while(current_start_send < n)
            {
        MPI_Status status;
        MPI_Recv(&recieve_temp, 100, MPI_DOUBLE, MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        /*MPI_Get_count(&status, MPI_INT, &count);*/
        /*//printf("worker ID: %d; task ID: %d; count: %d\n",status.source, status.tag, count);*/
        recieve_ans = recieve_ans + recieve_temp;
        /*//send one more chunk*/
        MPI_Send(&current_start_send, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        current_start_send = current_start_send + chunk;
      }

      int end_flag = -1;
      /*//after there is no chunks left to send*/
      for (int i = 1; i < size; ++i)
      {
        MPI_Send(&end_flag, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      }

    }
    else
    {
      /*//code for worker threads*/
      while(1)
      {
        double intermediate_result, ans = 0;
        int start_iteration;
        MPI_Recv(&start_iteration, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        if (start_iteration == -1)
        {
          MPI_Finalize();
          exit(0);
        }
        int end_iteration = start_iteration + chunk;
        for (int i = start_iteration; i < end_iteration; ++i)
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

        MPI_Send(&ans, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }

    }



    if (rank == 0)
    {
      gettimeofday(&end, NULL);
      t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
      time_spent = t2-t1;
    }


   /*printf("Node %d has executed from %d to %d iterations\n", rank, start_iteration, end_iteration );
 * */

MPI_Finalize();
if (rank == 0) {
    printf("\n N: %d, operation_intensity: %d, Cores: %d  Time spent : %f", n, intensity, size, time_spent);
}
return EXIT_SUCCESS;
}