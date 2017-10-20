#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

  int ans = 0, a, b, n, intensity, thread_ans[20], number_of_threads, choice = 1;
  int loop_done = 0, granularity, current_iteration = 0;
  pthread_mutex_t thread_granularity_lock, thread_ans_lock, thread_current_iteration_lock, thread_ans_array_lock;
int get_start_iteration()
{
    return current_iteration;
}

int get_end_iteration()
{
  pthread_mutex_lock(&thread_current_iteration_lock);
  current_iteration += granularity;
  pthread_mutex_unlock(&thread_current_iteration_lock);
    return current_iteration;

}

void* run_thread()
{

	/*Implementing Loop Scheduler*/
  int begin,end;
  double intermediate_result;
  pthread_t thread_id = pthread_self();
  pthread_mutex_lock(&thread_ans_array_lock);
  thread_ans[thread_id]=0;
  pthread_mutex_unlock(&thread_ans_array_lock);
  while(loop_done != 1)
  {
    
    pthread_mutex_lock(&thread_granularity_lock);
    begin = get_start_iteration();
    end = get_end_iteration();
    pthread_mutex_unlock(&thread_granularity_lock);

    double loop_ans;
    loop_ans = 0;
    for (int i = begin; i < end; ++i)
   {
     /*Caluclating differential equivalent*/
    intermediate_result = (a+(i+0.5)*((b-a)/n));
    /*function = x^2*/
      intermediate_result = intermediate_result * intermediate_result;
      intermediate_result = intermediate_result*((b-a)/n);

      /*Adding Loop Answer to global variable according to choice provided by user*/
      if(choice == 1)
      {
      pthread_mutex_lock(&thread_ans_lock);
      ans += intermediate_result;
      pthread_mutex_unlock(&thread_ans_lock);
      }
      if(choice == 2)
      {
        loop_ans += intermediate_result;
      }
      if(choice == 3)
      {
      	pthread_mutex_lock(&thread_ans_array_lock);
        thread_ans[thread_id] += intermediate_result;
        pthread_mutex_unlock(&thread_ans_array_lock);
      }
      for (int k = 0; k < intensity; ++k)
        {  
        	printf("\n %lf", intermediate_result);
           intermediate_result = intermediate_result * intermediate_result;
           intermediate_result = sqrt(intermediate_result);
        }
   }
   if(choice == 2)
   {
      pthread_mutex_lock(&thread_ans_lock);
      ans += loop_ans;
      pthread_mutex_unlock(&thread_ans_lock);
   }

  }
}



int main(int argc, char *argv[]) {
  a = atoi(argv[1]);
  b = atoi(argv[2]);
  n = atoi(argv[3]);
  intensity = atoi(argv[4]);
  number_of_threads = atoi(argv[5]);
  choice = atoi(argv[6]);

  if (argc < 7)
  {
  	printf("\n Please input 6 arguments.");
  	exit(0);
  }
  
  pthread_t thread_array[number_of_threads];

  /*Initializing Mutex's*/
  pthread_mutex_init(&thread_ans_lock, NULL);
  pthread_mutex_init(&thread_granularity_lock, NULL);
  pthread_mutex_init(&thread_current_iteration_lock, NULL);
  pthread_mutex_init(&thread_ans_array_lock, NULL);

  /*Code to caluclate Time*/
   clock_t begin = clock();
   for (int i = 0; i < number_of_threads; ++i)
   {
     pthread_create(&thread_array[i], NULL, run_thread, NULL);
   }

   for (int i = 0; i < number_of_threads; ++i)
   {
     pthread_join(thread_array[i], NULL);
     ans += thread_ans[i];
   }
   clock_t end = clock();
   double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

   printf("FInal Answer is: %lf", ans);

return EXIT_SUCCESS;
}