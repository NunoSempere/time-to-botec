#include "../squiggle.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with a seed of 0
    /*
    for (int i = 0; i < 100; i++) {
        double draw = sample_unit_uniform(seed);
        printf("%f\n", draw);

    }*/
    // Test division
    printf("\n%d\n", 10 % 3);

    free(seed);
}
