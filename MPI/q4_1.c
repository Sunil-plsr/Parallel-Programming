#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
	 int  a, b, n, intensity, recieve_temp, recieve_ans = 0, current_start_send = 0;
   int threads;
   double final_answer = 0;
  a = 1;
  b = 100;
  n = atoi(argv[1]);
  intensity = atoi(argv[2]);
  MPI_Request send_request,recv_request[3];

  int number_of_threads, choice, dynamic_length;
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int chunk = n/200;

  /*Timings*/
  double t1, t2, time_spent;
  struct timeval start, end;


  if (rank == 0)
  {
  	/*Start the timer*/
  	gettimeofday(&start, NULL);
    t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);

  	/*Firest it will divide the iterations to 200 equal parts and stores them  in an array*/
    int buffer_send_iterations[200], buffer_send_index = 0;
  	for (int i = 0; i * chunk < n+1; ++i)
   	{
   		buffer_send_iterations[i] = i * chunk;
   	}

   	/*Sends initial iterations to all of the slave nodes*/
   	for (int i = 1; i < size; ++i)
   	{
   		for (int j = 0; j < 3; ++j)
   		{
   			MPI_Isend(&buffer_send_iterations[buffer_send_index++], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &send_request);
   		}
   	}

   	/*Sends additional iterations to nodes which have completed processing one chunk of data*/
   	MPI_Request send_more_request, data;
   	while(buffer_send_iterations[buffer_send_index] < n - chunk)
   	{
   		MPI_Status status;
   		int msg_recieve_node_rank;
   		/*33 tag for recieveing rank*/
   		/*22 tag for recieveing answer from 33's rank-node*/
 		MPI_Irecv(&msg_recieve_node_rank,1,MPI_INT, MPI_ANY_SOURCE,33,MPI_COMM_WORLD,&send_more_request);
		MPI_Wait(&send_more_request, MPI_STATUS_IGNORE);
		/*Recieves ans*/
		double node_ans;
		MPI_Irecv(&node_ans,1,MPI_DOUBLE, msg_recieve_node_rank,22,MPI_COMM_WORLD,&data);
		MPI_Wait(&data, MPI_STATUS_IGNORE);
		final_answer = final_answer + node_ans;
		/*Sends new chunk of data*/
		MPI_Isend(&buffer_send_iterations[buffer_send_index++], 1, MPI_INT, msg_recieve_node_rank, 0, MPI_COMM_WORLD, &send_request);
   	}

   	/*Sends end signal to all nodes that work has completed*/
   	int end_signal = -1;
   	for (int i = 1; i < n; ++i)
   	{
		MPI_Isend(&end_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &send_request);
   	}
  }
  else
  {
  	int input_buffer_read_index = 0;
  	int buffer_recieve_iterations[300], buffer_recieve_index = 0;
  	for (int i = 0; i < 300; ++i)
  	{
  		buffer_recieve_iterations[i] = -5;
  	}
  	for (int i = 0; i < 3; ++i)
  	{
  		MPI_Irecv(&buffer_recieve_iterations[buffer_recieve_index++],1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request[i]);
  		if (buffer_recieve_index == 3)
  			buffer_recieve_index = 0;
  	}
	MPI_Wait(&recv_request[2], MPI_STATUS_IGNORE);

	double answer_send_buff[1000];
	int  answer_index = 0;
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
		MPI_Isend(&rank, 1, MPI_INT, 0, 33, MPI_COMM_WORLD, &send_request);
   		/*Sending ans to 0 node*/
   		MPI_Isend(&answer_send_buff[answer_index], 1, MPI_DOUBLE, 0, 22, MPI_COMM_WORLD, &send_request);

   		/*Recieves data*/
   		MPI_Irecv(&buffer_recieve_iterations[buffer_recieve_index++],1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request[0]);
  		
  		/*Waits if slave dosent recieve any further chunks from master*/
   		if (buffer_recieve_iterations[input_buffer_read_index] == -5)
   		{
   			MPI_Wait(&recv_request[0], MPI_STATUS_IGNORE);
   		}

	}

  	/*Display recieved indexex*/
/*	printf("Node %d recieved indexes: %d : %d : %d \n",rank, buffer_recieve_iterations[0], buffer_recieve_iterations[1], buffer_recieve_iterations[2] );
*/  
  
  }





            MPI_Finalize();
            if (rank == 0)
            {
            	gettimeofday(&end, NULL);
      			t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
      			time_spent = t2-t1;
      			printf("Time spent on %d nodes is: %f \n", size, time_spent);
            }


	return 0;
}