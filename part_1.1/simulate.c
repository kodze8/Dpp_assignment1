/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "simulate.h"



/* Add any global variables you may need. */
static const double c = 0.15;

/* Add any functions you may need (like a worker) here. */
    void rotate_arrays(double **old_array, double **current_array, double **next_array) {
        double *temp = *old_array;
        *old_array = *current_array;
        *current_array = *next_array;
        *next_array = temp;
    }
/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * num_threads: how many threads to use (excluding the main threads)
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */



/**-----------TEMPLATE-----------*/
// double *simulate(const int i_max, const int t_max, const int num_threads,
//         double *old_array, double *current_array, double *next_array)
// {
//     /*
//      * After each timestep, you should swap the buffers around. Watch out none
//      * of the threads actually use the buffers at that time.
//      */
//
//
//     /* You should return a pointer to the array with the final results. */
//     return current_array;
// }
/**-----------------------------*/



/**-----------Sequential Implementation-----------*/
//EXPERIMENT: Sequential code

double *simulateSequential_v1(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{
        for (int t = 0; t < t_max; t++) {
            for (int i = 1; i < i_max - 1; i++) {
                next_array[i] = 2 * current_array[i] - old_array[i]
                                + c * (current_array[i-1] - 2*current_array[i] + current_array[i+1]);
            }

            rotate_arrays(&old_array, &current_array, &next_array);
        }
        return current_array;
}
/**----------------------------------------*/


/**-----------Concurrent Implementation Without Barriers-----------*/
//EXPERIMENT: Chunk_Threading Approach: threads are not reused
typedef struct {
    int start, end;
    double *prev_array;
    double *current_array;
    double *next_array;
} WorkerArgs_v2;

void* worker_v2(void* arg) {
    WorkerArgs_v2 *args = (WorkerArgs_v2*) arg;
    for (int i = args->start; i < args->end; i++) {
       args->next_array[i] = 2  * args->current_array[i]
                                - args->prev_array[i]
                                +  c * (args->current_array[i-1] - 2*args->current_array[i] +  args->current_array[i+1]);
    }
    return NULL;
}

double *simulate_v2(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{
    pthread_t threads[num_threads];
    WorkerArgs_v2 args[num_threads];

    const int total_interior_points = i_max - 2;  // Points we actually compute
    const int chunk_size = total_interior_points / num_threads;
    const int remainder = total_interior_points % num_threads;

    for (int t = 0; t < t_max; t++) {
        int start_index = 1;

        for (int thr = 0; thr < num_threads; thr++) {
            int range = chunk_size;
            if (thr < remainder) {
                range++;
            }

            args[thr].start = start_index;
            args[thr].end = start_index + range;

            if (args[thr].end > i_max - 1 || thr==num_threads-1) {
                args[thr].end = i_max - 1;
            }

            args[thr].prev_array = old_array;
            args[thr].current_array = current_array;
            args[thr].next_array = next_array;

            pthread_create(&threads[thr], NULL, worker_v2, &args[thr]);

            start_index+=range;
        }
        for (int th = 0; th < num_threads; th++)
            pthread_join(threads[th], NULL);
        rotate_arrays(&old_array, &current_array, &next_array);
    }
    return current_array;
}
// 1000 100 2 sin
// Took 0.004433 seconds
// Normalized: 4.433e-08 seconds
/**----------------------------------------*/


/**-----------Concurrent Implementation With Barriers-----------*/
//EXPERIMENT: Chunk_Threading Approach with Barriers
typedef struct {
    int id;
    int i_max;
    int t_max;
    int start, end;

    double **old_array;
    double **current_array;
    double **next_array;

    pthread_barrier_t *barrier;
} WorkerArgs;

void* worker(void* arg) {
    WorkerArgs *args = (WorkerArgs*) arg;
    for (int t = 0; t < args->t_max; t++) {
        pthread_barrier_wait(args->barrier);

        // worker chunk computation
        for (int i = args->start; i < args->end; i++) {
            if (i > 0 && i < args->i_max - 1) {
                (*args->next_array)[i] = 2 * (*args->current_array)[i]
                                       - (*args->old_array)[i]
                                       + c * ((*args->current_array)[i-1]
                                            - 2 * (*args->current_array)[i]
                                            + (*args->current_array)[i+1]);
            }
        }

        // wait for other computations
        pthread_barrier_wait(args->barrier);

        if (args->id == 0) {
            rotate_arrays(args->old_array, args->current_array, args->next_array);
        }

        // wait for rotation
        pthread_barrier_wait(args->barrier);
    }

    return NULL;
}

double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{
    pthread_t threads[num_threads];
    WorkerArgs args[num_threads];
    pthread_barrier_t barrier;

    // create barrier for all threads
    pthread_barrier_init(&barrier, NULL, num_threads);

    const int total_interior_points = i_max - 2;
    const int chunk_size = total_interior_points / num_threads;
    const int remainder = total_interior_points % num_threads;

    int start_index = 1;

    // worker threads
    for (int thr = 0; thr < num_threads; thr++) {
        int range = chunk_size;
        if (thr < remainder) {
            range++;
        }

        args[thr].id = thr;
        args[thr].i_max = i_max;
        args[thr].t_max = t_max;
        args[thr].start = start_index;
        args[thr].end = start_index + range;

        // edge case
        if (thr == num_threads - 1) {
            args[thr].end = i_max - 1;
        }

        // Pass pointers to array pointers for swapping
        args[thr].old_array = &old_array;
        args[thr].current_array = &current_array;
        args[thr].next_array = &next_array;
        args[thr].barrier = &barrier;

        // start worker thread
        pthread_create(&threads[thr], NULL, worker, &args[thr]);
        start_index += range;
    }

    // for all threads to complete all timesteps
    for (int th = 0; th < num_threads; th++) {
        pthread_join(threads[th], NULL);
    }

    // clean barrier
    pthread_barrier_destroy(&barrier);
    return current_array;
}
// 1000 100 2 sin
// Took 0.00164351 seconds
// Normalized: 1.64351e-08 seconds
/**----------------------------------------*/




