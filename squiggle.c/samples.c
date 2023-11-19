#include "squiggle_c/squiggle.h"
#include "squiggle_c/squiggle_more.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    double p_a = 0.8;
    double p_b = 0.5;
    double p_c = p_a * p_b;

    double sample_0(uint64_t* seed){ return 0; }
    double sample_1(uint64_t* seed) { return 1; } 
    double sample_few(uint64_t* seed) { return sample_to(1, 3, seed); } 
    double sample_many(uint64_t* seed) { return sample_to(2, 10, seed); } 

    int n_dists = 4;
    double weights[] = { 1 - p_c, p_c / 2, p_c / 4, p_c / 4 };
    double (*samplers[])(uint64_t*) = { sample_0, sample_1, sample_few, sample_many };
    double sampler_result(uint64_t* seed) { 
        return sample_mixture(samplers, weights, n_dists, seed);
    } 

    int n_samples = 1000 * 1000, n_threads = 16;
    double* results = malloc(n_samples * sizeof(double));
    parallel_sampler(sampler_result, results, n_threads, n_samples);
    printf("Avg: %f\n", array_sum(results, n_samples)/n_samples);
    free(results);
}
