#include "../../../squiggle.h"
#include "../../../squiggle_more.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

double probability_of_dying_nuno(uint64_t* seed)
{
    double first_year_russian_nuclear_weapons = 1953;
    double current_year = 2022;
    double laplace_probability_nuclear_exchange_year = sample_beta(1, current_year - first_year_russian_nuclear_weapons + 1, seed);
    double laplace_probability_nuclear_exchange_month = 1 - pow(1 - laplace_probability_nuclear_exchange_year, (1.0 / 12.0));

    double london_hit_conditional_on_russia_nuclear_weapon_usage = sample_beta(7.67, 69.65, seed);
    // I.e., a beta distribution with a range of 0.05 to 0.16 into: https://nunosempere.com/blog/2023/03/15/fit-beta/
    // 0.05 were my estimate and Samotsvety's estimate in March 2022, respectively:
    // https://forum.effectivealtruism.org/posts/KRFXjCqqfGQAYirm5/samotsvety-nuclear-risk-forecasts-march-2022#Nu_o_Sempere
    double informed_actor_not_able_to_escape = sample_beta(3.26212166586967, 3.26228162008564, seed);
    // 0.2 to 0.8, i.e., 20% to 80%, again using the previous tool
    double proportion_which_die_if_bomb_drops_in_london = sample_beta(10.00, 2.45, seed); // 60% to 95%

    double probability_of_dying = laplace_probability_nuclear_exchange_month * london_hit_conditional_on_russia_nuclear_weapon_usage * informed_actor_not_able_to_escape * proportion_which_die_if_bomb_drops_in_london;
    return probability_of_dying;
}

double probability_of_dying_eli(uint64_t* seed)
{
    double russia_nato_nuclear_exchange_in_next_month = sample_beta(1.30, 1182.99, seed); // .0001 to .003
    double london_hit_conditional = sample_beta(3.47, 8.97, seed); // 0.1 to 0.5
    double informed_actors_not_able_to_escape = sample_beta(2.73, 5.67, seed); // .1 to .6
    double proportion_which_die_if_bomb_drops_in_london = sample_beta(3.00, 1.46, seed); // 0.3 to 0.95;

    double probability_of_dying = russia_nato_nuclear_exchange_in_next_month * london_hit_conditional * informed_actors_not_able_to_escape * proportion_which_die_if_bomb_drops_in_london;
    return probability_of_dying;
}

double mixture(uint64_t* seed)
{
    double (*samplers[])(uint64_t*) = { probability_of_dying_nuno, probability_of_dying_eli };
    double weights[] = { 0.5, 0.5 };
    return sample_mixture(samplers, weights, 2, seed);
}

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    int n = 1000 * 1000;

    double* mixture_result = malloc(sizeof(double) * n);
    for (int i = 0; i < n; i++) {
        mixture_result[i] = mixture(seed);
    }

    printf("mixture_result: [ ");
    for (int i = 0; i < 9; i++) {
        printf("%.6f, ", mixture_result[i]);
    }
    printf("... ]\n");

    ci ci_90 = get_90_confidence_interval(mixture, seed);
    printf("mean: %f\n", array_mean(mixture_result, n));
    printf("90%% confidence interval: [%f, %f]\n", ci_90.low, ci_90.high);

    free(seed);
}
