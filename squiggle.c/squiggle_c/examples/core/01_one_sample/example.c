#include "../../../squiggle.h"
#include <stdio.h>
#include <stdlib.h>

// Estimate functions
double sample_0(uint64_t* seed)
{
    return 0;
}

double sample_1(uint64_t* seed)
{
    return 1;
}

double sample_few(uint64_t* seed)
{
    return sample_to(1, 3, seed);
}

double sample_many(uint64_t* seed)
{
    return sample_to(2, 10, seed);
}

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    double p_a = 0.8;
    double p_b = 0.5;
    double p_c = p_a * p_b;

    int n_dists = 4;
    double weights[] = { 1 - p_c, p_c / 2, p_c / 4, p_c / 4 };
    double (*samplers[])(uint64_t*) = { sample_0, sample_1, sample_few, sample_many };

    double result_one = sample_mixture(samplers, weights, n_dists, seed);
    printf("result_one: %f\n", result_one);
    free(seed);
}
