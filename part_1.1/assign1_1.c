/*
 * assign1_1.c
 *
 * Contains code for setting up and finishing the simulation.
 * NOTE: YOU SHOULD IMPLEMENT NOT HAVE TO LOOK HERE, IMPLEMENT YOUR CODE IN
 *       simulate.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "file.h"
#include "timer.h"
#include "simulate.h"

typedef double (*func_t)(double x);

/*
 * Simple gauss with mu=0, sigma^1=1
 */
double gauss(double x)
{
    return exp((-1 * x * x) / 2);
}


/*
 * Fills a given array with samples of a given function. This is used to fill
 * the initial arrays with some starting data, to run the simulation on.
 *
 * The first sample is placed at array index `offset'. `range' samples are
 * taken, so your array should be able to store at least offset+range doubles.
 * The function `f' is sampled `range' times between `sample_start' and
 * `sample_end'.
 */
void fill(double *array, int offset, int range, double sample_start,
        double sample_end, func_t f)
{
    int i;
    float dx;

    dx = (sample_end - sample_start) / range;
    for (i = 0; i < range; i++) {
        array[i + offset] = f(sample_start + i * dx);
    }
}


// int main(int argc, char *argv[])
// {
//     double *old, *current, *next, *ret;
//     int t_max, i_max, num_threads;
//     double time;
//
//     /* Parse commandline args: i_max t_max num_threads */
//     if (argc < 4) {
//         printf("Usage: %s i_max t_max num_threads [initial_data]\n", argv[0]);
//         printf(" - i_max: number of discrete amplitude points, should be >2\n");
//         printf(" - t_max: number of discrete timesteps, should be >=1\n");
//         printf(" - num_threads: number of threads to use for simulation, "
//                 "should be >=1\n");
//         printf(" - initial_data: select what data should be used for the first "
//                 "two generation.\n");
//         printf("   Available options are:\n");
//         printf("    * sin: one period of the sinus function at the start.\n");
//         printf("    * sinfull: entire data is filled with the sinus.\n");
//         printf("    * gauss: a single gauss-function at the start.\n");
//         printf("    * file <2 filenames>: allows you to specify a file with on "
//                 "each line a float for both generations.\n");
//
//         return EXIT_FAILURE;
//     }
//
//     i_max = atoi(argv[1]);
//     t_max = atoi(argv[2]);
//     num_threads = atoi(argv[3]);
//
//     if (i_max < 3) {
//         printf("argument error: i_max should be >2.\n");
//         return EXIT_FAILURE;
//     }
//     if (t_max < 1) {
//         printf("argument error: t_max should be >=1.\n");
//         return EXIT_FAILURE;
//     }
//     if (num_threads < 1) {
//         printf("argument error: num_threads should be >=1.\n");
//         return EXIT_FAILURE;
//     }
//
//     /* Allocate and initialize buffers. */
//     old = malloc(i_max * sizeof(double));
//     current = malloc(i_max * sizeof(double));
//     next = malloc(i_max * sizeof(double));
//
//     if (old == NULL || current == NULL || next == NULL) {
//         fprintf(stderr, "Could not allocate enough memory, aborting.\n");
//         return EXIT_FAILURE;
//     }
//
//     memset(old, 0, i_max * sizeof(double));
//     memset(current, 0, i_max * sizeof(double));
//     memset(next, 0, i_max * sizeof(double));
//
//     /* How should we will our first two generations? */
//     if (argc > 4) {
//         if (strcmp(argv[4], "sin") == 0) {
//             fill(old, 1, i_max/4, 0, 2*3.14, sin);
//             fill(current, 2, i_max/4, 0, 2*3.14, sin);
//         } else if (strcmp(argv[4], "sinfull") == 0) {
//             fill(old, 1, i_max-2, 0, 10*3.14, sin);
//             fill(current, 2, i_max-3, 0, 10*3.14, sin);
//         } else if (strcmp(argv[4], "gauss") == 0) {
//             fill(old, 1, i_max/4, -3, 3, gauss);
//             fill(current, 2, i_max/4, -3, 3, gauss);
//         } else if (strcmp(argv[4], "file") == 0) {
//             if (argc < 7) {
//                 printf("No files specified!\n");
//                 return EXIT_FAILURE;
//             }
//             file_read_double_array(argv[5], old, i_max);
//             file_read_double_array(argv[6], current, i_max);
//         } else {
//             printf("Unknown initial mode: %s.\n", argv[4]);
//             return EXIT_FAILURE;
//         }
//     } else {
//         /* Default to sinus. */
//         fill(old, 1, i_max/4, 0, 2*3.14, sin);
//         fill(current, 2, i_max/4, 0, 2*3.14, sin);
//     }
//
//     timer_start();
//
//     /* Call the actual simulation that should be implemented in simulate.c. */
//     ret = simulate(i_max, t_max, num_threads, old, current, next);
//
//     time = timer_end();
//     printf("Took %g seconds\n", time);
//     printf("Normalized: %g seconds\n", time / (i_max * t_max));
//
//     file_write_double_array("result.txt", ret, i_max);
//
//     free(old);
//     free(current);
//     free(next);
//
//     return EXIT_SUCCESS;
// }


/**-----------------------------------------------------------------------------------------------*/

//EXPERIMENT CODE

typedef double* (*simulate_func_t)(const int, const int, const int,
                                   double*, double*, double*);

void runner(simulate_func_t sim_func, int t_max, int i_max, int num_threads, double* out_time) {
    double *old, *current, *next;
    double time;

    /* Allocate buffers */
    old = malloc(i_max * sizeof(double));
    current = malloc(i_max * sizeof(double));
    next = malloc(i_max * sizeof(double));
    if (!old || !current || !next) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    memset(old, 0, i_max * sizeof(double));
    memset(current, 0, i_max * sizeof(double));
    memset(next, 0, i_max * sizeof(double));

    /* Initialize first two generations (sinus by default) */
    fill(old, 1, i_max / 4, 0, 2*3.14, sin);
    fill(current, 2, i_max / 4, 0, 2*3.14, sin);

    timer_start();
    sim_func(i_max, t_max, num_threads, old, current, next);  // choose implementation based on version if needed
    time = timer_end();

    *out_time = time;  // return elapsed time

    free(old);
    free(current);
    free(next);
}


void experiment1(void) {
    FILE *fp = fopen("./results_ex1.csv", "w");
    if (!fp) {
        perror("Could not open results_ex1.csv");
        return;
    }

    fprintf(fp, "simulate_version,i_max,t_max,num_threads,time_seconds,normalized_time\n");

    //EXPERIMENT 1: check report for more details
    int i_max_values[] = {1000, 10000, 100000, 1000000, 10000000};
    int num_i = sizeof(i_max_values) / sizeof(i_max_values[0]);

    int num_threads = 4;
    int t_max = 100;

    for (int version = 1; version <= 3; version++) {
        for (int i = 0; i < num_i; i++) {
            int i_max = i_max_values[i];
            double elapsed_time;


            if (version==1)
                runner(simulateSequential_v1, t_max, i_max, num_threads, &elapsed_time);
            else  if (version==2)
                runner(simulate_v2, t_max, i_max, num_threads, &elapsed_time);
            else
                runner(simulate, t_max, i_max, num_threads, &elapsed_time);

            // Write results to CSV
            fprintf(fp, "%d,%d,%d,%d,%g,%g\n",
                    version, i_max, t_max, num_threads, elapsed_time, elapsed_time / (i_max * t_max));

            printf("Version %d, i_max=%d -> %g seconds\n", version, i_max, elapsed_time);
        }
    }

    fclose(fp);
    printf("All results saved to results_ex1.csv\n");
}

void experiment2(void) {
    FILE *fp = fopen("./results_ex2.csv", "w");
    if (!fp) {
        perror("Could not open results_ex2.csv");
        return;
    }

    fprintf(fp, "i_max,t_max,num_threads,time_seconds,speedup\n");

    // You can adjust these
    int i_max_values[] = {1000, 100000, 1000000};
    int t_max_values[] = {100, 1000, 4000};

    int num_i = sizeof(i_max_values) / sizeof(i_max_values[0]);
    int num_t = sizeof(t_max_values) / sizeof(t_max_values[0]);

    int thread_counts[] = {1, 2, 4, 6, 8, 12, 14, 16};
    int num_threads_options = sizeof(thread_counts) / sizeof(thread_counts[0]);


    for (int a = 0; a < num_i; a++) {
        for (int b = 0; b < num_t; b++) {

            int i_max = i_max_values[a];
            int t_max = t_max_values[b];

            double base_time = 0.0;

            printf("\n--- i_max=%d, t_max=%d ---\n", i_max, t_max);

            for (int tc = 0; tc < num_threads_options; tc++) {

                int num_threads = thread_counts[tc];
                double elapsed_time;

                // run simulation (always using optimized v3)
                runner(simulate, t_max, i_max, num_threads, &elapsed_time);

                // first run (1 thread) becomes baseline
                if (num_threads == 1)
                    base_time = elapsed_time;

                double speedup = base_time / elapsed_time;

                fprintf(fp, "%d,%d,%d,%g,%g\n",
                        i_max, t_max, num_threads, elapsed_time, speedup);

                printf("Threads=%d -> %g seconds, Speedup=%.2fx\n",
                       num_threads, elapsed_time, speedup);
            }
        }
    }

    fclose(fp);
    printf("All results saved to results_ex2.csv\n");
}



int main(void)
{
    experiment2();
    return 0;
}