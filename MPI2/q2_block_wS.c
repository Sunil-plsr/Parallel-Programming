#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>

/*Block version of matrix multiplication*/

/*Here "recieve_x_horizontal()" will reduce all of the x values in a horizontal row to its left most node, and all of the left most nodes will 
concatinate x vector usind "collate_x_to_root()" method, so that node 0 will have final result of x vector before iteration completes, if there is another teration
node 0 will broadcast the latest x values to all other nodes before heat equation starts*/

int row_start, row_end, rows, n, cols;
int64_t **arr, x[800400], x_previous[800400], recv_buff[800400];
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

void copy_to_prev()
{
  for (int i = 0; i < n; ++i)
  {
    x_previous[i] = x[i];
  }
}



void recieve_x_horizontal()
{ int square_root = sqrt(size);
  if (((rank == 0) || (rank%square_root == 0)) && (rank != 1))
  {
    int recv_buf_index = 0;
    for (int i = 1; i<sqrt(size); ++i)
    {
      recv_buf_index=0;
      MPI_Recv(&recv_buff[recv_buf_index], cols, MPI_LONG_LONG, rank+i,MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      /*printf("%d is recieving from %d\n",rank, rank+i );*/
      for (int i = 0; i < cols; ++i)
      {
        x[i] = x[i] + recv_buff[i];
      }
    }
  }
  else
  {
    /*int offset = (size%(rank+1));*/
    int dest = 0;
    if (rank < square_root)
    {
      /*printf("%d is sending to 0\n", rank);*/
      MPI_Send(&x[0], cols, MPI_LONG_LONG, 0 , 0, MPI_COMM_WORLD);
    }
    else
    {
      int dest = rank - 1;
      while(dest%square_root != 0)
        dest--;
      /*printf("%d is sending to %d\n",rank, dest );*/
      MPI_Send(&x[0], cols, MPI_LONG_LONG, dest , 0, MPI_COMM_WORLD);
    }
    /*MPI_Send(&x[0], cols, MPI_LONG_LONG, size/(rank+1) , 0, MPI_COMM_WORLD);*/
  }
}

int collate_x_to_root()
{
  int square_root = sqrt(size);
  if (rank == 0)
  {
    int x_index = cols;
    for (int i = 1; i < size; ++i)
    {
      if (i%square_root == 0)
      {
        /*printf("0 recieves from %d\n", i);*/
        MPI_Recv(&x[x_index], cols, MPI_LONG_LONG, i,MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        x_index += cols;
      }
    }
  }
  else if (rank%square_root == 0)
  {
    /*printf("%d is sending to 0\n", rank);*/
    MPI_Send(&x[0], cols, MPI_LONG_LONG, 0 , 0, MPI_COMM_WORLD);
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


  double t1, t2, time_spent;
  struct timeval start, end;
  gettimeofday(&start, NULL);
  t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);

  broadcast_x_prev();
    
  while(iterations--)
  {
    compute_x();
    recieve_x_horizontal();
    collate_x_to_root();
    if (rank == 0)
      copy_to_prev();
    broadcast_x_prev();
  }

  gettimeofday(&end, NULL);
  t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
  time_spent = t2-t1;

  if(rank == 0)
  printf("\n N: %d, Cores: %d  Time spent : %f", n, size, time_spent);


  MPI_Finalize();
  free_matrix();

  return 0;
}