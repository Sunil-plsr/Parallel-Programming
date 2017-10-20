#include <unistd.h>
#include <mpi.h>
#include <stdio.h>


int main(int argc, char ** argv)
{
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  printf("I am %d of %d\n", rank, size);


  char hostname[128];

  gethostname(hostname, 128);
  printf("My hostname: %s\n and rank is: %d", hostname, rank);



  MPI_Finalize();

  return 0;
}