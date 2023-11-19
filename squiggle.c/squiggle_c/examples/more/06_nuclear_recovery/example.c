#include "../../../squiggle.h"
#include "../../../squiggle_more.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

double yearly_probability_nuclear_collapse(double year, uint64_t* seed)
{
    double successes = 0;
    double failures = (year - 1960);
    return sample_laplace(successes, failures, seed);
    // ^ can change to (successes + 1)/(trials + 2)
    // to get a probability,
    // rather than sampling from a distribution over probabilities.
}
double yearly_probability_nuclear_collapse_2023(uint64_t* seed)
{
    return yearly_probability_nuclear_collapse(2023, seed);
}

double yearly_probability_nuclear_collapse_after_recovery(double year, double rebuilding_period_length_years, uint64_t* seed)
{
    // assumption: nuclear
    double successes = 1.0;
    double failures = (year - rebuilding_period_length_years - 1960 - 1);
    return sample_laplace(successes, failures, seed);
}
double yearly_probability_nuclear_collapse_after_recovery_example(uint64_t* seed)
{
    double year = 2070;
    double rebuilding_period_length_years = 30;
    // So, there was a nuclear collapse in 2040,
    // then a recovery period of 30 years
    // and it's now 2070
    return yearly_probability_nuclear_collapse_after_recovery(year, rebuilding_period_length_years, seed);
}

double yearly_probability_nuclear_collapse_after_recovery_antiinductive(uint64_t* seed)
{
    return yearly_probability_nuclear_collapse(2023, seed) / 2;
}

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    int num_samples = 1000000;

    // Before a first nuclear collapse
    printf("## Before the first nuclear collapse\n");
    ci ci_90_2023 = get_90_confidence_interval(yearly_probability_nuclear_collapse_2023, seed);
    printf("90%% confidence interval: [%f, %f]\n", ci_90_2023.low, ci_90_2023.high);

    double* yearly_probability_nuclear_collapse_2023_samples = malloc(sizeof(double) * num_samples);
    for (int i = 0; i < num_samples; i++) {
        yearly_probability_nuclear_collapse_2023_samples[i] = yearly_probability_nuclear_collapse_2023(seed);
    }
    printf("mean: %f\n", array_mean(yearly_probability_nuclear_collapse_2023_samples, num_samples));

    // After the first nuclear collapse
    printf("\n## After the first nuclear collapse\n");
    ci ci_90_2070 = get_90_confidence_interval(yearly_probability_nuclear_collapse_after_recovery_example, seed);
    printf("90%% confidence interval: [%f, %f]\n", ci_90_2070.low, ci_90_2070.high);

    double* yearly_probability_nuclear_collapse_after_recovery_samples = malloc(sizeof(double) * num_samples);
    for (int i = 0; i < num_samples; i++) {
        yearly_probability_nuclear_collapse_after_recovery_samples[i] = yearly_probability_nuclear_collapse_after_recovery_example(seed);
    }
    printf("mean: %f\n", array_mean(yearly_probability_nuclear_collapse_after_recovery_samples, num_samples));

    // After the first nuclear collapse (antiinductive)
    printf("\n## After the first nuclear collapse (antiinductive)\n");
    ci ci_90_antiinductive = get_90_confidence_interval(yearly_probability_nuclear_collapse_after_recovery_antiinductive, seed);
    printf("90%% confidence interval: [%f, %f]\n", ci_90_antiinductive.low, ci_90_antiinductive.high);

    double* yearly_probability_nuclear_collapse_after_recovery_antiinductive_samples = malloc(sizeof(double) * num_samples);
    for (int i = 0; i < num_samples; i++) {
        yearly_probability_nuclear_collapse_after_recovery_antiinductive_samples[i] = yearly_probability_nuclear_collapse_after_recovery_antiinductive(seed);
    }
    printf("mean: %f\n", array_mean(yearly_probability_nuclear_collapse_after_recovery_antiinductive_samples, num_samples));

    free(seed);
}
