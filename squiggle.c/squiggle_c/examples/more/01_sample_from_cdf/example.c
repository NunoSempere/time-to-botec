#include "../../../squiggle.h"
#include "../../../squiggle_more.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_SAMPLES 1000000

// Example cdf
double cdf_uniform_0_1(double x)
{
    if (x < 0) {
        return 0;
    } else if (x > 1) {
        return 1;
    } else {
        return x;
    }
}

double cdf_squared_0_1(double x)
{
    if (x < 0) {
        return 0;
    } else if (x > 1) {
        return 1;
    } else {
        return x * x;
    }
}

double cdf_normal_0_1(double x)
{
    double mean = 0;
    double std = 1;
    return 0.5 * (1 + erf((x - mean) / (std * sqrt(2)))); // erf from math.h
}

// Some testers
void test_inverse_cdf_double(char* cdf_name, double cdf_double(double))
{
    struct box result = inverse_cdf_double(cdf_double, 0.5);
    if (result.empty) {
        printf("Inverse for %s not calculated\n", cdf_name);
        exit(1);
    } else {
        printf("Inverse of %s at %f is: %f\n", cdf_name, 0.5, result.content);
    }
}

void test_and_time_sampler_double(char* cdf_name, double cdf_double(double), uint64_t* seed)
{
    printf("\nGetting some samples from %s:\n", cdf_name);
    clock_t begin = clock();
    for (int i = 0; i < NUM_SAMPLES; i++) {
        struct box sample = sampler_cdf_double(cdf_double, seed);
        if (sample.empty) {
            printf("Error in sampler function for %s", cdf_name);
        } else {
            // printf("%f\n", sample.content);
        }
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f\n", time_spent);
}

int main()
{
    // Test inverse cdf double
    test_inverse_cdf_double("cdf_uniform_0_1", cdf_uniform_0_1);
    test_inverse_cdf_double("cdf_squared_0_1", cdf_squared_0_1);
    test_inverse_cdf_double("cdf_normal_0_1", cdf_normal_0_1);

    // Testing samplers
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    // Test double sampler
    test_and_time_sampler_double("cdf_uniform_0_1", cdf_uniform_0_1, seed);
    test_and_time_sampler_double("cdf_squared_0_1", cdf_squared_0_1, seed);
    test_and_time_sampler_double("cdf_normal_0_1", cdf_normal_0_1, seed);

    // Get some normal samples using a previous approach
    printf("\nGetting some samples from sample_unit_normal\n");

    clock_t begin_2 = clock();
    double* normal_samples = malloc(NUM_SAMPLES * sizeof(double));
    for (int i = 0; i < NUM_SAMPLES; i++) {
        normal_samples[i] = sample_unit_normal(seed);
        // printf("%f\n", normal_sample);
    }

    clock_t end_2 = clock();
    double time_spent_2 = (double)(end_2 - begin_2) / CLOCKS_PER_SEC;
    printf("Time spent: %f\n", time_spent_2);

    free(seed);
    return 0;
}
