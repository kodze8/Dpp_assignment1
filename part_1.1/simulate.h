/*
 * simulate.h
 */

#pragma once

double *simulate(const int i_max, const int t_max, const int num_cpus,
        double *old_array, double *current_array, double *next_array);


// added for testing
double *simulateSequential_v1(const int i_max, const int t_max, const int num_threads,
                              double *old_array, double *current_array, double *next_array);

double *simulate_v2(const int i_max, const int t_max, const int num_threads,
                    double *old_array, double *current_array, double *next_array);

