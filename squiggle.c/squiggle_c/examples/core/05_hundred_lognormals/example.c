#include "../../../squiggle.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Estimate functions
int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    for (int i = 0; i < 100; i++) {
        double sample = sample_lognormal(0, 10, seed);
        printf("%f\n", sample);
    }
    free(seed);
}
