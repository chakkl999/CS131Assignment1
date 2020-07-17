#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

/*Global variables */
int num_threads;
pthread_mutex_t *mutexes;

/* For representing the status of each philosopher */
typedef enum{
    none,   // No forks
    one,    // One fork
    two     // Both forks to consume
} utensil;

/* Representation of a philosopher */
typedef struct phil_data{
    int phil_num;
    int course;
    utensil forks;
    time_t meal_time[3];
}phil_data;

time_t start_time;

/* ****************Change function below ***************** */
void* eat_meal(void* phil){
/* 3 course meal: Each need to acquire both forks 3 times.
 *  First try for fork in front.
 * Then for the one on the right, if not fetched, put the first one back.
 * If both acquired, eat one course.
 */
    phil_data* philosopher = (phil_data*)phil;
    while(philosopher->course < 3){
        pthread_mutex_lock(&mutexes[philosopher->phil_num%num_threads]);
        philosopher->forks = one;
        if(!pthread_mutex_trylock(&mutexes[(philosopher->phil_num+1)%num_threads])){
            sleep(1);
            philosopher->forks = two;
            time(&philosopher->meal_time[philosopher->course]);
            philosopher->course++;
            pthread_mutex_unlock(&mutexes[(philosopher->phil_num+1)%num_threads]);
        }
        philosopher->forks = none;
        pthread_mutex_unlock(&mutexes[philosopher->phil_num%num_threads]);
    }
}

/* ****************Add the support for pthreads in function below ***************** */
int main( int argc, char **argv ){
    int num_philosophers, error;

    if (argc < 2) {
          fprintf(stderr, "Format: %s <Number of philosophers>\n", argv[0]);
          return 0;
     }

    num_philosophers = num_threads = atoi(argv[1]);
    pthread_t *threads = malloc(sizeof(pthread_t)*num_threads);
    phil_data *philosophers = malloc(sizeof(phil_data)*num_philosophers); //Struct for each philosopher
    mutexes = malloc(sizeof(pthread_mutex_t)*num_philosophers); //Each mutex element represent a fork

    /* Initialize structs */
    for( int i = 0; i < num_philosophers; i++ ){
        philosophers[i].phil_num = i;
        philosophers[i].course   = 0;
        philosophers[i].forks    = none;
    }
    /* Syntax to record time.
    While printing meal times for each philosopher, subtract start_time from recorded values */
    time(&start_time);

/* Each thread will represent a philosopher */

/* Initialize Mutex, Create threads, Join threads and Destroy mutex */
    for(int i = 0; i < num_philosophers; ++i) {
        pthread_mutex_init(&mutexes[i], NULL);
    }
    for(int i = 0; i < num_philosophers; ++i) {
        pthread_create(&threads[i], NULL, &eat_meal, &philosophers[i]);
    }
    for(int i = 0; i < num_philosophers; ++i) {
        pthread_join(threads[i], NULL);
    }
    for(int i = 0; i < num_philosophers; ++i) {
        pthread_mutex_destroy(&mutexes[i]);
    }
//    printf("Current time = %i\n", start_time);
    for(int i = 0; i < num_philosophers; ++i) {
        for(int j = 0; j < 3; ++j) {
            printf("Philosopher: %i ate course %i in %i seconds.\n", i, j, philosophers[i].meal_time[j] - start_time);
        }
        printf("\n");
    }
    return 0;
}
