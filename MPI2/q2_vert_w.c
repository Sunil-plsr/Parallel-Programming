#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>


/*Vertical version of matrix multiplication*/

/*Here after each node completes its iteration, reduce_X() is used to summate all x values in node 0
after which node 0 broadcasts x value using broadcast_x_prev() method*/

int col_start, col_end, rows, n, cols, rank, size;
int64_t **arr, x[800400], x_previous[800400], recvbuf[800400];


void initialize_matrix(){
  arr = (int64_t**)malloc(rows*sizeof(int64_t*));
/*  buffer_up_row = (int64_t*)malloc(cols*sizeof(int64_t*));
  buffer_down_row = (int64_t*)malloc(cols*sizeof(int64_t*));
*/  
  for(int64_t i=0;i< rows;i++)
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
    for (int64_t i = 0; i < n; ++i)
    {
      x_previous[i] = rand()%10;
    }
  }

}


void summate_all_x(){
  printf("Summating function on rank %d\n", rank);
  if (rank == 0)
  {
    for (int64_t i = 1; i < size; ++i)
    {
      MPI_Recv(&recvbuf[0], n, MPI_LONG_LONG, i,MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for (int64_t j = 0; j < n; ++j)
      { 
        x[j] = x[j] + recvbuf[j];
      }
    }
  }
  else
  {
    MPI_Send(&x[0], n, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD); 
  }
}

void free_matrix()
{
  for(int64_t i=0;i<rows;i++)
  {
      free(arr[i]);
  }
  free(arr);
}


void reduce_x()
{
  /*int64_t MPI_Reduce(const void *sendbuf, void *recvbuf, int64_t count, MPI_Datatype datatype,
               MPI_Op op, int64_t root, MPI_Comm comm)*/
  MPI_Reduce(&x[0], &x_previous, n, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
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

void copy_to_x_prev()
{
  for (int64_t i = 0; i < n; ++i)
  {
    x_previous[i] = x[i];
  }
}




int main(int argc, char ** argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  /*printf("I am %d of %d\n", rank, size);*/

  int64_t iterations = 20;
  n = atoi(argv[1]);
  col_start = (n/size) * rank;
  col_end = (n/size) * (rank+1);
  rows = n;
  cols = col_end - col_start;
  initialize_matrix();


  double t1, t2, time_spent;
  struct timeval start, end;
  gettimeofday(&start, NULL);
  t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);

  broadcast_x_prev();
    
  while(iterations--)
  {
    compute_x();
    /*summate_all_x();
    if (rank == 0)
      copy_to_x_prev();*/
    reduce_x();
    broadcast_x_prev();
  }
/*
  if (rank == 0)
  {
    for (int64_t i = 0; i < n; ++i)
    {
      printf("%d, ", x_previous[i]);
    }
  }
*/

  MPI_Barrier(MPI_COMM_WORLD);

    gettimeofday(&end, NULL);
  t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
  time_spent = t2-t1;

  if(rank == 0)
  printf("\n N: %d, Cores: %d  Time spent : %f", n, size, time_spent);


  MPI_Finalize();
  free_matrix();
  return 0;
}