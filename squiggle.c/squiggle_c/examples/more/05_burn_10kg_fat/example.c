#include "../../../squiggle.h"
#include "../../../squiggle_more.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double sample_minutes_per_day_jumping_rope_needed_to_burn_10kg(uint64_t* seed)
{
    double kcal_jumping_rope_minute = sample_to(15, 20, seed);
    double kcal_jumping_rope_hour = kcal_jumping_rope_minute * 60;

    double kcal_in_kg_of_fat = 7700;
    double num_kg_of_fat_to_lose = 10;

    double hours_jumping_rope_needed = kcal_in_kg_of_fat * num_kg_of_fat_to_lose / kcal_jumping_rope_hour;

    double days_until_end_of_year = 152; // as of 2023-08-01
    double hours_per_day = hours_jumping_rope_needed / days_until_end_of_year;
    double minutes_per_day = hours_per_day * 60;
    return minutes_per_day;
}

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    int n = 1000 * 1000;

    double* result = malloc(sizeof(double) * n);
    for (int i = 0; i < n; i++) {
        result[i] = sample_minutes_per_day_jumping_rope_needed_to_burn_10kg(seed);
    }

    printf("## How many minutes per day do I have to jump rope to lose 10kg of fat by the end of the year?\n");
    printf("Mean: %f\n", array_mean(result, n));
    printf("A few samples: [ ");
    for (int i = 0; i < 9; i++) {
        printf("%.6f, ", result[i]);
    }
    printf("... ]\n");

    ci ci_90 = get_90_confidence_interval(sample_minutes_per_day_jumping_rope_needed_to_burn_10kg, seed);
    printf("90%% confidence interval: [%f, %f]\n", ci_90.low, ci_90.high);

    free(seed);
}
