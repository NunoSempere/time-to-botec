#include "../../../squiggle.h"
#include "../../../squiggle_more.h"
#include <stdio.h>
#include <stdlib.h>

double sample_0(uint64_t* seed)
{
    return 0;
}

double sample_1(uint64_t* seed)
{
    return 1;
}

double sample_normal_mean_1_std_2(uint64_t* seed)
{
    return sample_normal(1, 2, seed);
}

double sample_1_to_3(uint64_t* seed)
{
    return sample_to(1, 3, seed);
}

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    int n_dists = 4;
    double weights[] = { 1, 2, 3, 4 };
    double (*samplers[])(uint64_t*) = {
        sample_0,
        sample_1,
        sample_normal_mean_1_std_2,
        sample_1_to_3
    };

    int n_samples = 10;
    for (int i = 0; i < n_samples; i++) {
        printf("Sample #%d: %f\n", i, sample_mixture(samplers, weights, n_dists, seed));
    }

    free(seed);
}
