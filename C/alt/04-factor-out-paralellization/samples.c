#include <math.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const float PI = 3.14159265358979323846;

#define N 1000000

//Array helpers

void array_print(float* array, int length)
{
    for (int i = 0; i < length; i++) {
        printf("item[%d] = %f\n", i, array[i]);
    }
    printf("\n");
}

float array_sum(float* array, int length)
{
    float output = 0.0;
    for (int i = 0; i < length; i++) {
        output += array[i];
    }
    return output;
}

void array_cumsum(float* array_to_sum, float* array_cumsummed, int length)
{
    array_cumsummed[0] = array_to_sum[0];
    for (int i = 1; i < length; i++) {
        array_cumsummed[i] = array_cumsummed[i - 1] + array_to_sum[i];
    }
}

// Split array helpers
int split_array_get_length(int index, int total_length, int n_threads)
{
    return (total_length % n_threads > index ? total_length / n_threads + 1 : total_length / n_threads);
}

void split_array_allocate(float** meta_array, int length, int divide_into)
{
    int split_array_length;

    for (int i = 0; i < divide_into; i++) {
        split_array_length = split_array_get_length(i, length, divide_into);
        meta_array[i] = malloc(split_array_length * sizeof(float));
    }
}

void split_array_free(float** meta_array, int divided_into)
{
    for (int i = 0; i < divided_into; i++) {
        free(meta_array[i]);
    }
    free(meta_array);
}

float split_array_sum(float** meta_array, int length, int divided_into)
{
    int i;
    float output = 0;

#pragma omp parallel for reduction(+ \
                                   : output)
    for (int i = 0; i < divided_into; i++) {
        float own_partial_sum = 0;
        int split_array_length = split_array_get_length(i, length, divided_into);
        for (int j = 0; j < split_array_length; j++) {
            own_partial_sum += meta_array[i][j];
        }
        output += own_partial_sum;
    }
    return output;
}

// Pseudo Random number generator

uint32_t xorshift32(uint32_t* seed)
{
    // Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
    // See <https://stackoverflow.com/questions/53886131/how-does-xorshift32-works>
    // https://en.wikipedia.org/wiki/Xorshift
    // Also some drama: <https://www.pcg-random.org/posts/on-vignas-pcg-critique.html>, <https://prng.di.unimi.it/>

    uint32_t x = *seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return *seed = x;
}

// Distribution & sampling functions

float rand_0_to_1(uint32_t* seed)
{
    return ((float)xorshift32(seed)) / ((float)UINT32_MAX);
    /* 
	uint32_t x = *seed;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return ((float)(*seed = x))/((float) UINT32_MAX);
	*/
    // previously:
    // ((float)rand_r(seed) / (float)RAND_MAX)
    // and before that: rand, but it wasn't thread-safe.
    // See: <https://stackoverflow.com/questions/43151361/how-to-create-thread-safe-random-number-generator-in-c-using-rand-r> for why to use rand_r:
    // rand() is not thread-safe, as it relies on (shared) hidden seed.
}

float rand_float(float max, uint32_t* seed)
{
    return rand_0_to_1(seed) * max;
}

float ur_normal(uint32_t* seed)
{
    float u1 = rand_0_to_1(seed);
    float u2 = rand_0_to_1(seed);
    float z = sqrtf(-2.0 * log(u1)) * sin(2 * PI * u2);
    return z;
}

float random_uniform(float from, float to, uint32_t* seed)
{
    return rand_0_to_1(seed) * (to - from) + from;
}

float random_normal(float mean, float sigma, uint32_t* seed)
{
    return (mean + sigma * ur_normal(seed));
}

float random_lognormal(float logmean, float logsigma, uint32_t* seed)
{
    return expf(random_normal(logmean, logsigma, seed));
}

float random_to(float low, float high, uint32_t* seed)
{
    const float NORMAL95CONFIDENCE = 1.6448536269514722;
    float loglow = logf(low);
    float loghigh = logf(high);
    float logmean = (loglow + loghigh) / 2;
    float logsigma = (loghigh - loglow) / (2.0 * NORMAL95CONFIDENCE);
    return random_lognormal(logmean, logsigma, seed);
}

// Mixture function

float mixture(float (*samplers[])(uint32_t*), float* weights, int n_dists, uint32_t* seed)
{

    // You can see a slightly simpler version of this function in the git history
    // or in alt/C-02-better-algorithm-one-thread/
    float sum_weights = array_sum(weights, n_dists);
    float* cumsummed_normalized_weights = malloc(n_dists * sizeof(float));
    cumsummed_normalized_weights[0] = weights[0] / sum_weights;
    for (int i = 1; i < n_dists; i++) {
        cumsummed_normalized_weights[i] = cumsummed_normalized_weights[i - 1] + weights[i] / sum_weights;
    }

    //create var holders
    float p1, result;
    int sample_index, i, own_length;
    p1 = random_uniform(0, 1, seed);
    for (int i = 0; i < n_dists; i++) {
        if (p1 < cumsummed_normalized_weights[i]) {
            result = samplers[i](seed);
            break;
        }
    }
    free(cumsummed_normalized_weights);
    return result;
}

// Parallization function
void paralellize(float (*sampler)(uint32_t* seed), float** results, int n_threads){

    int sample_index, i, split_array_length;
    uint32_t** seeds = malloc(n_threads * sizeof(uint32_t*));
    for (uint32_t i = 0; i < n_threads; i++) {
        seeds[i] = malloc(sizeof(uint32_t));
        *seeds[i] = i + 1; // xorshift can't start with 0
    }

    #pragma omp parallel private(i, sample_index, split_array_length)
    {
        #pragma omp for
        for (i = 0; i < n_threads; i++) {
            split_array_length = split_array_get_length(i, N, n_threads);
            for (int j = 0; j < split_array_length; j++) {
                results[i][j] = sampler(seeds[i]);
            }
        }
    }

    for (uint32_t i = 0; i < n_threads; i++) {
        free(seeds[i]);
    }
    free(seeds);
}

// Functions used for the BOTEC.
// Their type has to be the same, as we will be passing them around.

float sample_0(uint32_t* seed)
{
    return 0;
}

float sample_1(uint32_t* seed)
{
    return 1;
}

float sample_few(uint32_t* seed)
{
    return random_to(1, 3, seed);
}

float sample_many(uint32_t* seed)
{
    return random_to(2, 10, seed);
}

float sample_mixture(uint32_t* seed){
    float p_a, p_b, p_c;

    // Initialize variables
    p_a = 0.8;
    p_b = 0.5;
    p_c = p_a * p_b;

    // Generate mixture
    int n_dists = 4;
    float weights[] = { 1 - p_c, p_c / 2, p_c / 4, p_c / 4 };
    float (*samplers[])(uint32_t*) = { sample_0, sample_1, sample_few, sample_many };

    return mixture(samplers, weights, n_dists, seed);
}

int main()
{
    int n_threads = omp_get_max_threads();
    // printf("Max threads: %d\n", n_threads);
    // omp_set_num_threads(n_threads);
    float** split_array_results = malloc(n_threads * sizeof(float*));
    split_array_allocate(split_array_results, N, n_threads);

    paralellize(sample_mixture, split_array_results, n_threads);
    printf("Sum(split_array_results, N)/N = %f\n", split_array_sum(split_array_results, N, n_threads) / N);

    split_array_free(split_array_results, n_threads);
    return 0;
}
