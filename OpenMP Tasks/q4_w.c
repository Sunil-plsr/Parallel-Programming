#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

/*
Initializing dependency[28][28] array with dependecy values from strassens graph
if [i][j] has value 1 then task i is dependent on task j
completed_tasks[28] is an array which is intialized to 0 and if any task has completed execution then that corresponding index number value will be changed to 1

*/


int completed_tasks[28] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int intensity_val = 1000000000;
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
    
    
double do_work(int i)
{   
        double temp_var = 1234;
        /*Checking if the dependency is multiplication or addition*/
        if (14 <= i <= 20)
        {
                /* Multiplication Intensity */
                for (int Intensity = 0; Intensity < intensity_val*5; ++Intensity)
                {
                        temp_var = temp_var + 0.020023;
                }
                completed_tasks[i] = 1;
        printf("temporary var val is: %f\n ",temp_var);
        }
        else
        {
                /* Addition Intensity */
                for (int Intensity = 0; Intensity < intensity_val; ++Intensity)
                {
                        temp_var = temp_var + (1.354678*i*0.000000000234);
                }
                completed_tasks[i] = 1;
        printf("temo var:%f\n",temp_var);
        }

        /*Updating dependency matrix array so that other tasks which are dependent on current task will know*/
        for (int k = 0; k < 28; ++k)
        {
                dependency[k][i] = 0;
        }
}


int main(int argc, char const *argv[])
{

        /*Initializing dependency matrix to represent Depencencies present in Strassens algorithm DAG*/
        int completed = 0;

        int number_of_threads;

  /*  check_cmd_arguments(1, argc);
 *  */
    number_of_threads = atoi(argv[1]);
intensity_val = atoi(argv[2]);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    double t1 = (double)start.tv_sec+ (double)(start.tv_usec/1000000.0);
   // omp_set_num_threads(number_of_threads);

    #pragma omp parallel
       {
           #pragma omp single
           {
                while(completed == 0)
                {
               for (int i = 0; i < 28; ++i)
                                {

                                        int flag = 0;
                                        /*Checking if the task has any dependencies*/
                                        for (int g = 0; g < 28; ++g)
                                        {
                                                if (dependency[i][g] == 1)
                                                {
                                                        flag = 1;
                                                }
                                        }
                                        if(completed_tasks[i] == 1)
                                                flag=1;
                                        if (flag == 0)
                                        {
                                                #pragma omp task
                                                do_work(i);
                                        printf("completed task for %d\n",i);
                                        }

                                }


                                #pragma omp taskwait
                                completed = 1;
                                /*Checking if all tasks are completed or not*/
                                for (int i = 0; i < 28; ++i)
                                {
                                        for (int j = 0; j < 28; ++j)
                                        {
                                                if (dependency[i][j] == 1)
                                                {
                                                        completed = 0;
                                                }
                                        }
                                }
                        }



           }
       }
        gettimeofday(&end, NULL);
    double t2 = (double)end.tv_sec+ (double)(end.tv_usec/1000000.0);
    double time_spent = t2-t1;


    printf("\n Threads: %d :: Time: %f",number_of_threads, time_spent);


        return 0;
}
