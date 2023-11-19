#include "../../../squiggle.h"
#include "../../../squiggle_more.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Estimate functions
double beta_1_2_sampler(uint64_t* seed)
{
    return sample_beta(1, 2.0, seed);
}

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    ci beta_1_2_ci_90 = get_90_confidence_interval(beta_1_2_sampler, seed);
    printf("90%% confidence interval of beta(1,2) is [%f, %f]\n", beta_1_2_ci_90.low, beta_1_2_ci_90.high);

    free(seed);
}
