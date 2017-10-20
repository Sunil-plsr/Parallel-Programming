#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

/*Global declerations*/
int rank, size, chunk, a, b, n, intensity, recieve_temp, recieve_ans = 0, current_start_send = 0;
int buffer_send_iterations[200], buffer_send_index = 0;
double final_answer = 0, t1, t2;
int end_signal = -1;
MPI_Request send_request;
MPI_Request recv_request[200];
struct timeval start, end;


/*Worker Variables*/
int input_buffer_read_index = 0;
int buffer_recieve_iterations[200], buffer_recieve_index = 0;



void send_initial_iterations()
{
	/*here we will send initial iterations to all of the worker threads*/
	for (int i = 1; i < size; ++i)
		for (int j = 0; j < 3; ++j)
	  		MPI_Isend(&buffer_send_iterations[buffer_send_index++], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &send_request);
}

void send_additional_chunks()
{
	while(buffer_send_iterations[buffer_send_index] < n - chunk)
   	{
   		MPI_Status status;
   		int msg_recieve_node_rank;
   		MPI_Request send_more_request, data;
   		/*33 tag for recieveing rank*/
   		/*22 tag for recieveing answer from 33's rank-node*/
 		/*
    MPI_Irecv(&msg_recieve_node_rank,1,MPI_INT, MPI_ANY_SOURCE,33,MPI_COMM_WORLD,&send_more_request);
		MPI_Wait(&send_more_request, status);
		*/
    /*Recieves ans*/
		double node_ans;
		MPI_Irecv(&node_ans,1,MPI_DOUBLE, MPI_ANY_SOURCE,22,MPI_COMM_WORLD,&data);
    MPI_Wait(&data, status);
		/*Sends new chunk of data*/
		MPI_Isend(&buffer_send_iterations[buffer_send_index++], 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &send_request);
		/*Waiting to get ans so that it can be aggregated to final answer*/
		
		final_answer = final_answer + node_ans;		
   	}

}

void kill_threads()
{
   	for (int i = 1; i < size; ++i)
   	{
		MPI_Isend(&end_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &send_request);
   	}

}

void recieve_initial_iterations()
{
  	for (int i = 0; i < 200; ++i)
  	{
  		/*Initializing with a value so that we can check if the memory locations has no iterations in it*/
  		buffer_recieve_iterations[i] = -5;
  	}
  	for (int i = 0; i < 3; ++i)
  	{
  		MPI_Irecv(&buffer_recieve_iterations[buffer_recieve_index],1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request[buffer_recieve_index]);
      buffer_recieve_index++;
  	}
  	/*Waiting for worker to recieve so that it can start numerical integration*/
	MPI_Wait(&recv_request[0], MPI_STATUS_IGNORE);
}

void perform_numerical_integration()
{
	double answer_send_buff[1000];
	int answer_index = 0;
	while(buffer_recieve_iterations[input_buffer_read_index] != -1)
	{
		int start_iteration = buffer_recieve_iterations[input_buffer_read_index++];
		double ans = 0, intermediate_result;
		/*Numerical Integration code*/
		int end_iteration = start_iteration + chunk;
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
		answer_send_buff[answer_index] = ans;

		/*Sending rank to 0 rank*/
		/*MPI_Isend(&rank, 1, MPI_INT, 0, 33, MPI_COMM_WORLD, &send_request);*/
   		/*Sending ans to 0 node*/
   		MPI_Isend(&answer_send_buff[answer_index++], 1, MPI_DOUBLE, 0, 22, MPI_COMM_WORLD, &send_request);

      /*MPI_Request rec_req;*/
   		/*Recieves data*/
   		MPI_Irecv(&buffer_recieve_iterations[buffer_recieve_index],1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request[buffer_recieve_index]);
  		buffer_recieve_index++;
  		/*Waits if slave dosent recieve any further chunks from master*/
   		if (buffer_recieve_iterations[input_buffer_read_index] == -5)
   		{
   			MPI_Wait(&recv_request[input_buffer_read_index], MPI_STATUS_IGNORE);
   		}

	}

}

int main(int argc, char *argv[])
{
  int threads;
  a = 1;
  b = 100;
  n = atoi(argv[1]);
  intensity = atoi(argv[2]);

  int number_of_threads, choice, dynamic_length;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  chunk = n/200;

  if(rank == 0)
  {
  	/*Initialize iterations array*/
  	for (int i = 0; i * chunk < n+1; ++i)
   		buffer_send_iterations[i] = i * chunk;

    /*InitiALIZE time start*/
    gettimeofday(&start, NULL);
    t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);

  	send_initial_iterations();
  	send_additional_chunks();
  	kill_threads();
  }
  else
  {
  	recieve_initial_iterations();
  	perform_numerical_integration();
  }

  MPI_Finalize();
  if (rank == 0)
  {
    gettimeofday(&end, NULL);
	  t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
	  double time_spent = t2-t1;
	  printf("Time spent on %d nodes is: %f \n", size, time_spent);
  }
  	
}