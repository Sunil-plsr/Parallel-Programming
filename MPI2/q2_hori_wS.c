#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>

/*Horizontal version of matrix multiplication*/

/*all_gather_x() will gater all of the x values from every node to node 0

at the end final x vector will be available in node 0 in x_previous
*/
int row_start, row_end, rows, n, cols;
int64_t **arr, x[800400], x_previous[800400];
int rank, size;


void initialize_matrix(){
  arr = (int64_t**)malloc(rows*sizeof(int64_t*));
/*  buffer_up_row = (int64_t*)malloc(cols*sizeof(int64_t*));
  buffer_down_row = (int64_t*)malloc(cols*sizeof(int64_t*));
*/  
  for(int64_t i=0;i<rows;i++)
  {
      arr[i] = (int64_t*)malloc(cols*sizeof(int64_t));
  }

  for (int64_t i = 0; i < rows; ++i)
  {
    for (int64_t j = 0; j < cols; ++j)
    {
      arr[i][j] = rand()%10;  
    }
  }

  /*Initialize x_previous matrix*/
  if (rank == 0)
  {
    for (int64_t i = 0; i < cols; ++i)
    {
      x_previous[i] = rand()%10;
    }
  }

}

void free_matrix()
{
  for(int i=0;i<rows;i++)
  {
      free(arr[i]);
  }
  free(arr);
}

void compute_x()
{
  for (int64_t i = 0; i < rows; ++i)
  {
    x[i] = 0;
    for (int64_t j = 0; j < cols; ++j)
    {
      x[i] = x[i] + arr[i][j]*x_previous[j];
    }
  }
}

void broadcast_x_prev()
{
  MPI_Bcast(&x_previous[0], cols, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
}




void all_gather_x()
{
/*
  int64_t MPI_Allgather(const void *sendbuf, int64_t sendcount, MPI_Datatype sendtype,
                  void *recvbuf, int64_t recvcount, MPI_Datatype recvtype,
                  MPI_Comm comm)
*/
  MPI_Allgather(&x[0], rows, MPI_LONG_LONG, &x_previous[0], rows, MPI_LONG_LONG, MPI_COMM_WORLD);
}


int main(int argc, char ** argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  /*printf("I am %d of %d\n", rank, size);*/

  int64_t iterations = 20;
  n = atoi(argv[1]);
  row_start = (n/size) * rank;
  row_end = (n/size) * (rank+1);
  rows = row_end - row_start;
  cols = n;
  initialize_matrix();
  broadcast_x_prev();
    
  while(iterations--)
  {
    compute_x();
    all_gather_x();
  }

  MPI_Finalize();
  free_matrix();
  return 0;
}