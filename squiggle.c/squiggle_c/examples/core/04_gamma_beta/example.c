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

    int n = 1000 * 1000;
    /*
		for (int i = 0; i < n; i++) {
        double gamma_0 = sample_gamma(0.0, seed);
        // printf("sample_gamma(0.0): %f\n", gamma_0);
    }
		printf("\n");
		*/

    double* gamma_1_array = malloc(sizeof(double) * n);
    for (int i = 0; i < n; i++) {
        double gamma_1 = sample_gamma(1.0, seed);
        // printf("sample_gamma(1.0): %f\n", gamma_1);
        gamma_1_array[i] = gamma_1;
    }
    printf("gamma(1) summary statistics = mean: %f, std: %f\n", array_mean(gamma_1_array, n), array_std(gamma_1_array, n));
    free(gamma_1_array);
    printf("\n");

    double* beta_1_2_array = malloc(sizeof(double) * n);
    for (int i = 0; i < n; i++) {
        double beta_1_2 = sample_beta(1, 2.0, seed);
        // printf("sample_beta(1.0, 2.0): %f\n", beta_1_2);
        beta_1_2_array[i] = beta_1_2;
    }
    printf("beta(1,2) summary statistics: mean: %f, std: %f\n", array_mean(beta_1_2_array, n), array_std(beta_1_2_array, n));
    free(beta_1_2_array);
    printf("\n");

    free(seed);
}
