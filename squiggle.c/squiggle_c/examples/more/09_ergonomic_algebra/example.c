#include "../../../squiggle.h"
#include "../../../squiggle_more.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ln lognormal_params
#define to(...) convert_ci_to_lognormal_params((ci)__VA_ARGS__)
#define from(...) convert_lognormal_params_to_ci((ln)__VA_ARGS__)
#define times(a, b) algebra_product_lognormals(a, b)

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    ln a = to({ .low = 1, .high = 10 });
    ln b = to({ .low = 5, .high = 500 });
    ln c = times(a, b);

    printf("Result: to(%f, %f)\n", from(c).low, from(c).high);
    printf("One sample from it is: %f\n", sample_lognormal(c.logmean, c.logstd, seed));

    free(seed);
}
