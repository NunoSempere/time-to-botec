#include "squiggle_c/squiggle.h"
#include "squiggle_c/squiggle_more.h"
#include <stdio.h>
#include <stdlib.h>

double cumsum_p0 = 0.6;
double cumsum_p1 = 0.8;
double cumsum_p2 = 0.9;
double cumsum_p3 = 1.0;

double sampler_result(uint64_t * seed)
{

    double p = sample_unit_uniform(seed);
    if(p< cumsum_p0){
        return 0;
    } else if (p < cumsum_p1){
        return 1;
    } else if (p < cumsum_p2){
        return sample_to(1,3, seed);
    } else {
        return sample_to(2, 10, seed);
    } 
}

int main()
{

    int n_samples = 1000 * 1000, n_threads = 16;
    double* results = malloc((size_t)n_samples * sizeof(double));
    sampler_parallel(sampler_result, results, n_threads, n_samples);
    printf("Avg: %f\n", array_sum(results, n_samples) / n_samples);
    free(results);
}
