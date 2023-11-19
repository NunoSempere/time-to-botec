#include "../../../squiggle.h"
#include "../../../squiggle_more.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    // Convert to 90% confidence interval form and back
    lognormal_params ln1 = { .logmean = 1.0, .logstd = 3.0 };
    ci ln1_ci = convert_lognormal_params_to_ci(ln1);
    printf("The 90%% confidence interval of Lognormal(%f, %f) is [%f, %f]\n",
        ln1.logmean, ln1.logstd,
        ln1_ci.low, ln1_ci.high);
    lognormal_params ln1_params2 = convert_ci_to_lognormal_params(ln1_ci);
    printf("The lognormal which has 90%% confidence interval [%f, %f] is Lognormal(%f, %f)\n",
        ln1_ci.low, ln1_ci.high,
        ln1_params2.logmean, ln1_params2.logstd);

    lognormal_params ln2 = convert_ci_to_lognormal_params((ci) { .low = 1, .high = 10 });
    lognormal_params ln3 = convert_ci_to_lognormal_params((ci) { .low = 5, .high = 50 });

    lognormal_params sln = algebra_product_lognormals(ln2, ln3);
    ci sln_ci = convert_lognormal_params_to_ci(sln);

    printf("Result of some lognormal products: to(%f, %f)\n", sln_ci.low, sln_ci.high);

    free(seed);
}
