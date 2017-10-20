/*The program will output unique words and their word count to a file named "o"*/
#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace MAPREDUCE_NS;

void fileread(int, char *, KeyValue *, void *);
void sum(char *, int, char *, int, int *, KeyValue *, void *);


int main(int narg, char **args)
{
  MPI_Init(&narg,&args);

  int me,nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  if (narg <= 1) {
    if (me == 0) printf("Syntax: wordfreq file1 file2 ...\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  MapReduce *mr = new MapReduce(MPI_COMM_WORLD);
  mr->verbosity = 2;
 
  MPI_Barrier(MPI_COMM_WORLD);
  double tstart = MPI_Wtime();

  int nwords = mr->map(narg-1,&args[1],0,1,0,fileread,NULL);
  int nfiles = mr->mapfilecount;
  mr->collate(NULL);
  int nunique = mr->reduce(sum,NULL);
  /*My code*/
  /*Printing to console
  mr ->print(-1, 1, 5, 1);*/
  /*Gathering every node's data to node 0*/
  mr->gather(1);
  /*Printing to a file*/
  char outp = 'o';
  mr ->print(&outp, 1, 0, 1, 5, 1);

  MPI_Barrier(MPI_COMM_WORLD);
  double tstop = MPI_Wtime();

  delete mr;

  if (me == 0) {
    printf("%d total words, %d unique words\n",nwords,nunique);
    printf("Time to process %d files on %d procs = %g (secs)\n",
	   nfiles,nprocs,tstop-tstart);
  }

  MPI_Finalize();
}

/* ----------------------------------------------------------------------
   read a file
   for each word in file, emit key = word, value = NULL
------------------------------------------------------------------------- */

void fileread(int itask, char *fname, KeyValue *kv, void *ptr)
{
/*   filesize = # of bytes in file
*/
  struct stat stbuf;
  int flag = stat(fname,&stbuf);
  if (flag < 0) {
    printf("ERROR: Could not query file size\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }
  int filesize = stbuf.st_size;

  FILE *fp = fopen(fname,"r");
  char *text = new char[filesize+1];
  int nchar = fread(text,1,filesize,fp);
  text[nchar] = '\0';
  fclose(fp);

  char *whitespace = " \'\".,?<>;(){}[]!&\t\n\f\r\0";
  char *word = strtok(text,whitespace);
  while (word) {
    kv->add(tolower(word),strlen(word)+1,NULL,0);
    word = strtok(NULL,whitespace);
  }

  delete [] text;
}

/* ----------------------------------------------------------------------
   count word occurrence
   emit key = word, value = # of multi-values
------------------------------------------------------------------------- */

void sum(char *key, int keybytes, char *multivalue,
	 int nvalues, int *valuebytes, KeyValue *kv, void *ptr) 
{
  kv->add(key,keybytes,(char *) &nvalues,sizeof(int));
}