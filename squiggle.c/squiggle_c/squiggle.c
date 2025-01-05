#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

// Defs
#define PI 3.14159265358979323846 // M_PI in gcc gnu99
#define NORMAL90CONFIDENCE 1.6448536269514727
#define UNUSED(x) (void)(x)
// ^ https://stackoverflow.com/questions/3599160/how-can-i-suppress-unused-parameter-warnings-in-c

// Pseudo Random number generators
static uint64_t xorshift64(uint64_t* seed)
{
    // Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
    // See:
    //   <https://en.wikipedia.org/wiki/Xorshift>
    //   <https://stackoverflow.com/questions/53886131/how-does-xorshift32-works>,
    // Also some drama:
    //   <https://www.pcg-random.org/posts/on-vignas-pcg-critique.html>,
    //   <https://prng.di.unimi.it/>
    uint64_t x = *seed;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return *seed = x;

    /* 
    // if one wanted to generate 32 bit ints, 
    // from which to generate floats,
    // one could do the following: 
    uint32_t x = *seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return *seed = x;
    */
}

// Distribution & sampling functions
// Unit distributions
double sample_unit_uniform(uint64_t* seed)
{
    // samples uniform from [0,1] interval.
    return ((double)xorshift64(seed)) / ((double)UINT64_MAX);
}

double sample_unit_normal(uint64_t* seed)
{
    // // See: <https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform>
    double u1 = sample_unit_uniform(seed);
    double u2 = sample_unit_uniform(seed);
    double z = sqrt(-2.0 * log(u1)) * sin(2 * PI * u2);
    return z;
}

// Composite distributions
double sample_uniform(double start, double end, uint64_t* seed)
{
    return sample_unit_uniform(seed) * (end - start) + start;
}

double sample_normal(double mean, double sigma, uint64_t* seed)
{
    return (mean + sigma * sample_unit_normal(seed));
}

double sample_lognormal(double logmean, double logstd, uint64_t* seed)
{
    return exp(sample_normal(logmean, logstd, seed));
}

double sample_normal_from_90_ci(double low, double high, uint64_t* seed)
{
    // Explanation of key idea:
    // 1. We know that the 90% confidence interval of the unit normal is
    // [-1.6448536269514722, 1.6448536269514722]
    // see e.g.: https://stackoverflow.com/questions/20626994/how-to-calculate-the-inverse-of-the-normal-cumulative-distribution-function-in-p
    // or https://www.wolframalpha.com/input?i=N%5BInverseCDF%28normal%280%2C1%29%2C+0.05%29%2C%7B%E2%88%9E%2C100%7D%5D
    // 2. So if we take a unit normal and multiply it by
    // L / 1.6448536269514722, its new 90% confidence interval will be
    // [-L, L], i.e., length 2 * L
    // 3. Instead, if we want to get a confidence interval of length L,
    // we should multiply the unit normal by
    // L / (2 * 1.6448536269514722)
    // Meaning that its standard deviation should be multiplied by that amount
    // see: https://en.wikipedia.org/wiki/Normal_distribution?lang=en#Operations_on_a_single_normal_variable
    // 4. So we have learnt that Normal(0, L / (2 * 1.6448536269514722))
    // has a 90% confidence interval of length L
    // 5. If we want a 90% confidence interval from high to low,
    // we can set mean = (high + low)/2; the midpoint, and L = high-low,
    // Normal([high + low]/2, [high - low]/(2 * 1.6448536269514722))
    double mean = (high + low) / 2.0;
    double std = (high - low) / (2.0 * NORMAL90CONFIDENCE);
    return sample_normal(mean, std, seed);
}

double sample_to(double low, double high, uint64_t* seed)
{
    // Given a (positive) 90% confidence interval,
    // returns a sample from a lognorma with a matching 90% c.i.
    // Key idea: If we want a lognormal with 90% confidence interval [a, b]
    // we need but get a normal with 90% confidence interval [log(a), log(b)].
    // Then see code for sample_normal_from_90_ci
    double loglow = log(low);
    double loghigh = log(high);
    return exp(sample_normal_from_90_ci(loglow, loghigh, seed));
}

double sample_gamma(double alpha, uint64_t* seed)
{

    // A Simple Method for Generating Gamma Variables, Marsaglia and Wan Tsang, 2001
    // https://dl.acm.org/doi/pdf/10.1145/358407.358414
    // see also the references/ folder
    // Note that the Wikipedia page for the gamma distribution includes a scaling parameter
    // k or beta
    // https://en.wikipedia.org/wiki/Gamma_distribution
    // such that gamma_k(alpha, k) = k * gamma(alpha)
    // or gamma_beta(alpha, beta) = gamma(alpha) / beta
    // So far I have not needed to use this, and thus the second parameter is by default 1.
    if (alpha >= 1) {
        double d, c, x, v, u;
        d = alpha - 1.0 / 3.0;
        c = 1.0 / sqrt(9.0 * d);
        while (1) {

            do {
                x = sample_unit_normal(seed);
                v = 1.0 + c * x;
            } while (v <= 0.0);

            v = v * v * v;
            u = sample_unit_uniform(seed);
            if (u < 1.0 - 0.0331 * (x * x * x * x)) { // Condition 1
                // the 0.0331 doesn't inspire much confidence
                // however, this isn't the whole story
                // by knowing that Condition 1 implies condition 2
                // we realize that this is just a way of making the algorithm faster
                // i.e., of not using the logarithms
                return d * v;
            }
            if (log(u) < 0.5 * (x * x) + d * (1.0 - v + log(v))) { // Condition 2
                return d * v;
            }
        }
    } else {
        return sample_gamma(1 + alpha, seed) * pow(sample_unit_uniform(seed), 1 / alpha);
        // see note in p. 371 of https://dl.acm.org/doi/pdf/10.1145/358407.358414
    }
}

double sample_beta(double a, double b, uint64_t* seed)
{
    // See: https://en.wikipedia.org/wiki/Gamma_distribution#Related_distributions
    double gamma_a = sample_gamma(a, seed);
    double gamma_b = sample_gamma(b, seed);
    return gamma_a / (gamma_a + gamma_b);
}

double sample_laplace(double successes, double failures, uint64_t* seed)
{
    // see <https://en.wikipedia.org/wiki/Beta_distribution?lang=en#Rule_of_succession>
    return sample_beta(successes + 1, failures + 1, seed);
}

// Array helpers
double array_sum(double* array, int length)
{
    double sum = 0.0;
    for (int i = 0; i < length; i++) {
        sum += array[i];
    }
    return sum;
}

void array_cumsum(double* array_to_sum, double* array_cumsummed, int length)
{
    array_cumsummed[0] = array_to_sum[0];
    for (int i = 1; i < length; i++) {
        array_cumsummed[i] = array_cumsummed[i - 1] + array_to_sum[i];
    }
}

double array_mean(double* array, int length)
{
    double sum = array_sum(array, length);
    return sum / length;
}

double array_std(double* array, int length)
{
    double mean = array_mean(array, length);
    double std = 0.0;
    for (int i = 0; i < length; i++) {
        std += (array[i] - mean) * (array[i] - mean);
    }
    std = sqrt(std / length);
    return std;
}

// Mixture function
double sample_mixture(double (*samplers[])(uint64_t*), double* weights, int n_dists, uint64_t* seed)
{
    // Sample from samples with frequency proportional to their weights.
    double sum_weights = array_sum(weights, n_dists);
    double* cumsummed_normalized_weights = (double*)malloc((size_t)n_dists * sizeof(double));
    cumsummed_normalized_weights[0] = weights[0] / sum_weights;
    for (int i = 1; i < n_dists; i++) {
        cumsummed_normalized_weights[i] = cumsummed_normalized_weights[i - 1] + weights[i] / sum_weights;
    }

    double result;
    int result_set_flag = 0;
    double p = sample_uniform(0, 1, seed);
    for (int k = 0; k < n_dists; k++) {
        if (p < cumsummed_normalized_weights[k]) {
            result = samplers[k](seed);
            result_set_flag = 1;
            break;
        }
    }
    if (result_set_flag == 0)
        result = samplers[n_dists - 1](seed);

    free(cumsummed_normalized_weights);
    return result;
}
