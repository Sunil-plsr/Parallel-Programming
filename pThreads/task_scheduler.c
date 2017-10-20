/* Mockup strassens algorithm*/
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>


int task_queue_pointer = -1,complete_flag = 0, complete[28] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, task_queue[28];
int dependency[28][28]={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0}};
double temp_var = 1234;
pthread_mutex_t dependency_array_lock,complete_array_lock,task_queue_array_lock, complete_flag_lock, task_queue_pointer_lock;


void *master_thread_run(void *unused)
{
	int tasks_complete = 0;
	while(tasks_complete == 0)
	{
		tasks_complete = 1;
		for (int i = 0; i < 28; ++i)
		{   int row_flag = 1;
			/*row flag = 1 when the task has no dependencies
			i.e all of the elements in that row should be 0*/
			for (int j = 0; j < 28; ++j)
			{
				if (dependency[i][j] == 1)
				{
					row_flag = 0;
					tasks_complete = 0;
				}
			}
	
			if (row_flag == 1 && complete[i] == 0)
			{
				pthread_mutex_lock(&complete_array_lock);
				complete[i] = 1;
				pthread_mutex_unlock(&complete_array_lock);
				pthread_mutex_lock(&task_queue_array_lock);
				task_queue[++task_queue_pointer] = i;
				pthread_mutex_unlock(&task_queue_array_lock);
			}
		}
	}
	int over = 1;
	for (int i = 0; i < 28; ++i)
	{	
		if (complete[i] == 0)
		{
			over = 0;
		}
	}
	if (over == 1)
	{
		pthread_mutex_lock(&complete_flag_lock);
		complete_flag == 1;
		pthread_mutex_unlock(&complete_flag_lock);
	}
	
}

void *child_thread_run(void *unused)
{	int j;
	while(complete_flag == 0)
	{
		if (task_queue_pointer != 999)
		{	
			pthread_mutex_lock(&task_queue_array_lock);
			pthread_mutex_lock(&task_queue_pointer_lock);
			j = task_queue[task_queue_pointer--];
			pthread_mutex_unlock(&task_queue_pointer_lock);
			pthread_mutex_unlock(&task_queue_array_lock);
			if (14 <= j <= 20)
			{
				/* Multiplication Intensity */
				for (int Intensity = 0; Intensity < 500000; ++Intensity)
				{
					temp_var = temp_var * temp_var;
					temp_var = sqrt(temp_var);
				}
			}
			else
			{
				/* Addition Intensity */
				for (int Intensity = 0; Intensity < 100000; ++Intensity)
				{
					temp_var = temp_var * temp_var;
					temp_var = sqrt(temp_var);
				}	
			}
			for (int i = 0; i < 28; ++i)
			{
				if (dependency[i][j] == 1)
				{
					pthread_mutex_lock(&dependency_array_lock);
					dependency[i][j] = 0;
					pthread_mutex_unlock(&dependency_array_lock);
				}
			}
		}
	}
}

int main(int argc, char const *argv[])
{
	pthread_t master_thread, child_thread[atoi(argv[1])];
	/*We are using matrix to represent the dependencies between the processes, if a process is dependent 
	on another process it is represented by 1 in the matrix if it is not dependednt then value will be 0*/
	
		/*Index_Number = process name in DAG
		0-6   = m1-m7
		7-13  = n1-n7
		14-20 = p1-p7
		21-23 = t1-t3
		24-27 = c11-c22*/
	clock_t begin = clock();
	pthread_create(&master_thread, NULL, master_thread_run, NULL);
	for (int i = 0; i < atoi(argv[1]); ++i)
	{
		pthread_create(&child_thread[i], NULL, child_thread_run, NULL);
	}
	for (int i = 0; i < atoi(argv[1]); ++i)
	{
		pthread_join(child_thread[i], NULL);
	}
	clock_t end = clock();
   double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
   printf("\n Time taken on %d threads is: %f", atoi(argv[1]), time_spent);
	
}