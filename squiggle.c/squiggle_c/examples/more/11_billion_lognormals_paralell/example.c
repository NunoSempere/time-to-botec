#include "../../../squiggle.h"
#include "../../../squiggle_more.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Estimate functions
int main()
{
    // set randomness seed
    // uint64_t* seed = malloc(sizeof(uint64_t));
    // *seed = 1000; // xorshift can't start with 0
    // ^ not necessary, because parallel_sampler takes care of the seed.

    int n_samples = 1000 * 1000 * 1000;
    int n_threads = 16;
    double sampler(uint64_t* seed){
        return sample_lognormal(0, 10, seed);
    }
    double* results = malloc(n_samples * sizeof(double));

    parallel_sampler(sampler, results, n_threads, n_samples);
    double avg = array_sum(results, n_samples)/n_samples;
    printf("Average of 1B lognormal(0,10): %f", avg);

    free(results);

    // free(seed);
    // ^ not necessary, because parallel_sampler takes care of the seed.
}
