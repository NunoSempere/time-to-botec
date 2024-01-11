#ifndef SQUIGGLE_C_CORE
#define SQUIGGLE_C_CORE

// uint64_t header
#include <stdint.h>

// Pseudo Random number generator
uint64_t xorshift64(uint64_t* seed);

// Basic distribution sampling functions
double sample_unit_uniform(uint64_t* seed);
double sample_unit_normal(uint64_t* seed);

// Composite distribution sampling functions
double sample_uniform(double start, double end, uint64_t* seed);
double sample_normal(double mean, double sigma, uint64_t* seed);
double sample_lognormal(double logmean, double logsigma, uint64_t* seed);
double sample_normal_from_90_ci(double low, double high, uint64_t* seed);
double sample_to(double low, double high, uint64_t* seed);

double sample_gamma(double alpha, uint64_t* seed);
double sample_beta(double a, double b, uint64_t* seed);
double sample_laplace(double successes, double failures, uint64_t* seed);

// Array helpers
double array_sum(double* array, int length);
void array_cumsum(double* array_to_sum, double* array_cumsummed, int length);
double array_mean(double* array, int length);
double array_std(double* array, int length);

// Mixture function
double sample_mixture(double (*samplers[])(uint64_t*), double* weights, int n_dists, uint64_t* seed);

// Macro to mute "unused variable" warning when -Wall -Wextra is enabled. Useful for nested functions
#define UNUSED(x) (void)(x)

#endif
