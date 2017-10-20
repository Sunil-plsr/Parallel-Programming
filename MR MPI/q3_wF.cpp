/*

Sample output will be like the below:
KMV pair: proc 3, nvalues 5, sizes 4 21, key 1, values test this a Hello si

"nvalues 5, sizes 4 21, key 1," -> means 5 words appeared 1 times in the document.
*/




#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mapreduce.h>
#include <keyvalue.h>
#include <sys/stat.h>

using namespace MAPREDUCE_NS;

void fileread(int itask, char *fname, KeyValue *kv, void *ptr)
{
    struct stat stbuf;
    int flag = stat(fname,&stbuf);
    if (flag < 0) 
    {
        printf("ERROR: Could not query file size\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    int filesize = stbuf.st_size;

    FILE *fp = fopen(fname,"r");
    char *text = new char[filesize+1];
    int nchar = fread(text,1,filesize,fp);
    text[nchar] = '\0';
    fclose(fp);
    char *whitespace = " \'\".,?-!/#(){}[]:;<>\t\n\f\r\0";
    char *word = strtok(text,whitespace);
    while (word) 
    {
        kv->add(word,strlen(word)+1,NULL,0);
        word = strtok(NULL,whitespace);
    }
    delete [] text;
}

void sum(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, KeyValue *kv, void *ptr)
{
    kv->add((char *) &nvalues,sizeof(int),key,keybytes);
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc,&argv);

    int rank,P;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&P);

    if (argc <= 1) 
    {
        if (rank == 0) printf("Syntax: wordfreq file1 file2 ...\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    MapReduce *m = new MapReduce(MPI_COMM_WORLD);
    m->verbosity = 2;

    MPI_Barrier(MPI_COMM_WORLD);

    int nwords = m->map(argc-1,&argv[1],0,1,0,fileread,NULL);
    int nfiles = m->mapfilecount;
    m->collate(NULL);
    int nunique = m->reduce(sum,NULL); // changed the key value pair to value->key pair in the sum function
    m->collate(NULL);
    
    MPI_Barrier(MPI_COMM_WORLD);
      /*Printing to a file*/
    char *outp = new char[6];
    strncpy(outp, "output", sizeof(outp)-1);

    mr->gather(1);
    MPI_Barrier(MPI_COMM_WORLD);
    m->print(-1,1,1,5);


    char outp = 'o';
    m->print(&outp, 1, 0, 1, 1, 5);


    MPI_Barrier(MPI_COMM_WORLD);

    delete m;

    if (rank == 0) 
    {
        printf("%d total words, %d unique words\n",nwords,nunique);
    }

    MPI_Finalize();
}