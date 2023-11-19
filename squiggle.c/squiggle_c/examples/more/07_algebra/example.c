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

    normal_params n1 = { .mean = 1.0, .std = 3.0 };
    normal_params n2 = { .mean = 2.0, .std = 4.0 };
    normal_params sn = algebra_sum_normals(n1, n2);
    printf("The sum of Normal(%f, %f) and Normal(%f, %f) is Normal(%f, %f)\n",
        n1.mean, n1.std, n2.mean, n2.std, sn.mean, sn.std);

    lognormal_params ln1 = { .logmean = 1.0, .logstd = 3.0 };
    lognormal_params ln2 = { .logmean = 2.0, .logstd = 4.0 };
    lognormal_params sln = algebra_product_lognormals(ln1, ln2);
    printf("The product of Lognormal(%f, %f) and Lognormal(%f, %f) is Lognormal(%f, %f)\n",
        ln1.logmean, ln1.logstd, ln2.logmean, ln2.logstd, sln.logmean, sln.logstd);

    free(seed);
}
