#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>
/*Caluclations are made using rank to determine start and end iteration on which every node works on,
 * after completion each node answers are sent to node 0, in which each node answers are summed to get the final answer*/

int start_row, end_row;
int **arr, **prev, buffer_up_row[1000000], buffer_down_row[1000000], buffer_send1[1000000], buffer_send2[1000000];
int rows, cols;
int rank, size, n;
int *send_buffer;


void initialize_matrix()
{
  arr = (int**)malloc(rows*sizeof(int*));
  prev = (int**)malloc(rows*sizeof(int*));
/*  buffer_up_row = (int*)malloc(cols*sizeof(int*));
 *    buffer_down_row = (int*)malloc(cols*sizeof(int*));
 *    */
  for(int i=0;i<rows;i++)
  {
      arr[i] = (int*)malloc(cols*sizeof(int));
      prev[i] = (int*)malloc(cols*sizeof(int));
  }

  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      prev[i][j] = rand()%10;
    }
  }

}
void free_matrix()
{
  for(int i=0;i<rows;i++)
  {
      free(arr[i]);
      free(prev[i]);
  }
  free(arr);
  free(prev);
}

void copy_to_prev()
{
    for (int i = 0; i < rows; ++i)
    {
      for (int j = 0; j < cols; ++j)
      {
        prev[i][j] = arr[i][j];
      }
    }
}

void send_data_to_adjecent_node()
{
  if (rank == 0)
  {
    /*sends data to node 1*/
    for (int i = 0; i < cols; ++i)
    {
      buffer_send1[i] = prev[rows-1][i];
    }
    MPI_Send(&buffer_send1[0], cols, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
  }
  else if(rank == size-1)
  {
    /*Sends data to last but 1 node*/
    for (int i = 0; i < cols; ++i)
    {
      buffer_send1[i] = prev[0][i];
    }
    MPI_Send(&buffer_send1[0], cols, MPI_INT, rank-1, 0, MPI_COMM_WORLD);
  }
  else
  {
    /*Sends data to previous node*/
    for (int i = 0; i < cols; ++i)
    {
      buffer_send1[i] = prev[0][i];
    }
    MPI_Send(&buffer_send1[0], cols, MPI_INT, rank-1, 0, MPI_COMM_WORLD);
    /*Sends data to next node*/
    for (int i = 0; i < cols; ++i)
    {
      buffer_send2[i] = prev[rows-1][i];
    }
    MPI_Send(&buffer_send2[0], cols, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
  }
}

void get_data_from_adjecent_nodes()
{
  MPI_Status status;
  if (rank == 0)
  {
    /*Recieve data from node 1*/
    MPI_Recv(&buffer_down_row[0], cols, MPI_INT, rank+1,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }
  else if(rank == size-1)
  {
    /*Recieves data from last node*/
    MPI_Recv(&buffer_up_row[0], cols, MPI_INT, rank-1,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }
  else
  {
    /*Recieves data from previous node*/
    MPI_Recv(&buffer_up_row[0], cols, MPI_INT, rank-1,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    /*Recieves data from next node*/
    MPI_Recv(&buffer_down_row[0], cols, MPI_INT, rank+1,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }
}
void perform_heat()
{
  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      if (rank == 0 && i == 0)
      {
        /* if the row is first row in global matrix */
        if (j == 0)
        {
          /* if the element is left most in top edge (top left corner element) */
          arr[i][j] = (5*prev[i][j] + prev[i][j+1] +  prev[i+1][j] + prev[i+1][j+1])/9;
        }
        else if(j == cols-1)
        {
          /* if the element is right most in top edge (top right corner element) */
          arr[i][j] = (prev[i][j-1]  + 5*prev[i][j] +  prev[i+1][j-1] +  prev[i+1][j])/9;
        }
        else
        {
          /*If the element is in top edge but not in corners*/
          arr[i][j] = (prev[i][j-1]  + 3*prev[i][j] + prev[i][j+1] +  prev[i+1][j-1] +  prev[i+1][j] + prev[i+1][j+1])/9;
        }
      }
            else if (rank == size-1 && i == rows-1)
      {
        /* if the row is last row in global matrix */
        if (j == 0)
        {
          /* if the element is left most in bottom edge (bottom left corner element) */
          arr[i][j] = (prev[i-1][j] + prev[i-1][j+1] + 5*prev[i][j] + prev[i][j+1])/9;
        }
        else if (j == cols-1)
        {
          /* if the element is right most in bottom edge (bottom right corner element) */
          arr[i][j] = (prev[i-1][j-1] + prev[i-1][j] + prev[i][j-1] + 5*prev[i][j])/9;
        }
        else
        {
          /*If the element is in bottom edge but not in corners*/
          arr[i][j] = (prev[i-1][j-1] + prev[i-1][j] + prev[i-1][j+1] + prev[i][j-1] + 3*prev[i][j] + prev[i][j+1])/9;
        }
      }
      else if (i == 0)
      {
        if (j == 0)
        {
          /* if it is first element in forst row of sub-matrix */
          arr[i][j] = (buffer_up_row[j] + buffer_up_row[j+1] + 3*prev[i][j] + prev[i][j+1] +  prev[i+1][j] + prev[i+1][j+1])/9;
        }
        else if (j == cols-1)
        {
          /* if it is last element in forst row of sub-matrix */
          arr[i][j] = (buffer_up_row[j-1] + buffer_up_row[j] + prev[i][j-1] + 3*prev[i][j] +   prev[i+1][j-1] +  prev[i+1][j])/9;
        }
        else
        {
          /*If the element is in top row of submatrix but not in corners*/
          arr[i][j] = (buffer_up_row[j-1] + buffer_up_row[j] + buffer_up_row[j+1] + prev[i][j-1]
                        + prev[i][j] + prev[i][j+1] +  prev[i+1][j-1] +  prev[i+1][j] + prev[i+1][j+1])/9;
        }
      }
            else if (i == rows-1)
      {
        if (j == 0)
        {
          /* if it is first element in last row of sub-matrix */
          arr[i][j] = (prev[i-1][j] + prev[i-1][j+1] + 3*prev[i][j] + prev[i][j+1] +  buffer_down_row[j] + buffer_down_row[j+1])/9;
        }
        else if (j == cols-1)
        {
          /* if it is last element in last row of sub-matrix */
          arr[i][j] = (prev[i-1][j-1] + prev[i-1][j] + prev[i][j-1] + 3*prev[i][j] +   buffer_down_row[j-1] +  buffer_down_row[j])/9;
        }
        else
        {
          /*If the element is in bottom row of submatrix but not in corners*/
          arr[i][j] = (prev[i-1][j-1] + prev[i-1][j] + prev[i-1][j+1] + prev[i][j-1]
                        + prev[i][j] + prev[i][j+1] +  buffer_down_row[j-1] +  buffer_down_row[j] + buffer_down_row[j+1])/9;
        }
      }
      else
      {
        /*Default Case*/
        arr[i][j] = (prev[i-1][j-1] + prev[i-1][j] + prev[i-1][j+1] + prev[i][j-1]
                      + prev[i][j] + prev[i][j+1] +  prev[i+1][j-1] +  prev[i+1][j] + prev[i+1][j+1])/9;
      }

    }
  }
}

int main(int argc, char *argv[]) {
  int  a, b, intensity, recieve[32];
  int iterations = 20;
  n = atoi(argv[1]);

  /*Clauclation
 *   n = 15808 for 1gb of data*/


  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  start_row = (n/size) * rank;
  end_row = (n/size) * (rank + 1);
  rows = end_row - start_row;
  cols = n;
  initialize_matrix();

  double t1, t2, time_spent;
  struct timeval start, end;
  gettimeofday(&start, NULL);
  t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);

  while(iterations--)
  {
    if (size > 1)
    {
          if (rank%2 == 0)
          {
            send_data_to_adjecent_node();
            get_data_from_adjecent_nodes();
            perform_heat();
            copy_to_prev();
          }
          else
          {
            get_data_from_adjecent_nodes();
            send_data_to_adjecent_node();
            perform_heat();
            copy_to_prev();
          }
    }
    else
    {
      perform_heat();
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize();
  free_matrix();

  gettimeofday(&end, NULL);
  t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
  time_spent = t2-t1;

  if(rank == 0)
  printf("\n N: %d, Cores: %d  Time spent : %f", n, size, time_spent);





return EXIT_SUCCESS;
}