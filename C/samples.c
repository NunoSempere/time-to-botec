#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
int split_array_get_my_length(int index, int total_length, int n_threads)
{
    return (total_length % n_threads > index ? total_length / n_threads + 1 : total_length / n_threads);
}

void split_array_allocate(float** meta_array, int length, int divide_into)
{
    int own_length;

    for (int i = 0; i < divide_into; i++) {
        own_length = split_array_get_my_length(i, length, divide_into);
        meta_array[i] = malloc(own_length * sizeof(float));
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

    #pragma omp parallel for reduction(+:output)
		for (int i = 0; i < divided_into; i++) {
				float own_partial_sum = 0;
				int own_length = split_array_get_my_length(i, length, divided_into);
				for (int j = 0; j < own_length; j++) {
						own_partial_sum += meta_array[i][j];
				}
				output += own_partial_sum;
		}
		return output;

}

// Distribution & sampling functions

float rand_float(float to, unsigned int* seed)
{
    return ((float)rand_r(seed) / (float)RAND_MAX) * to;
    // See: <https://stackoverflow.com/questions/43151361/how-to-create-thread-safe-random-number-generator-in-c-using-rand-r> for why to use rand_r:
    // rand() is not thread-safe, as it relies on (shared) hidden state.
}

float ur_normal(unsigned int* seed)
{
    float u1 = rand_float(1.0, seed);
    float u2 = rand_float(1.0, seed);
    float z = sqrtf(-2.0 * log(u1)) * sin(2 * PI * u2);
    return z;
}

inline float random_uniform(float from, float to, unsigned int* seed)
{
    return ((float)rand_r(seed) / (float)RAND_MAX) * (to - from) + from;
}

inline float random_normal(float mean, float sigma, unsigned int* seed)
{
    return (mean + sigma * ur_normal(seed));
}

inline float random_lognormal(float logmean, float logsigma, unsigned int* seed)
{
    return expf(random_normal(logmean, logsigma, seed));
}

inline float random_to(float low, float high, unsigned int* seed)
{
    const float NORMAL95CONFIDENCE = 1.6448536269514722;
    float loglow = logf(low);
    float loghigh = logf(high);
    float logmean = (loglow + loghigh) / 2;
    float logsigma = (loghigh - loglow) / (2.0 * NORMAL95CONFIDENCE);
    return random_lognormal(logmean, logsigma, seed);
}

// Mixture function
void mixture(float (*samplers[])(unsigned int*), float* weights, int n_dists, float** results, int n_threads)
{
    // You can see a simpler version of this function in the git history
    // or in C-02-better-algorithm-one-thread/
    float sum_weights = array_sum(weights, n_dists);
		float* cumsummed_normalized_weights = malloc(n_dists * sizeof(float));
		cumsummed_normalized_weights[0] = weights[0]/sum_weights;
    for (int i = 1; i < n_dists; i++) {
        cumsummed_normalized_weights[i] = cumsummed_normalized_weights[i - 1] + weights[i]/sum_weights;
    }

    //create var holders
    float p1;
    int sample_index, i, own_length;
    
		// unsigned int* seeds[n_threads];
    unsigned int** seeds = malloc(n_threads * sizeof(unsigned int*));
		for (unsigned int i = 0; i < n_threads; i++) {
        seeds[i] = malloc(sizeof(unsigned int));
        *seeds[i] = i;
    }

    #pragma omp parallel private(i, p1, sample_index, own_length)
    {
        #pragma omp for
        for (i = 0; i < n_threads; i++) {
            own_length = split_array_get_my_length(i, N, n_threads);
            for (int j = 0; j < own_length; j++) {
                p1 = random_uniform(0, 1, seeds[i]);
                for (int k = 0; k < n_dists; k++) {
                    if (p1 < cumsummed_normalized_weights[k]) {
                        results[i][j] = samplers[k](seeds[i]);
                        break;
                    }
                }
            }
        }
    }
    // free(normalized_weights);
    // free(cummulative_weights);
		free(cumsummed_normalized_weights);
    for (unsigned int i = 0; i < n_threads; i++) {
        free(seeds[i]);
    }
		free(seeds);
}

// Functions used for the BOTEC.
// Their type has to be the same, as we will be passing them around.

float sample_0(unsigned int* seed)
{
    return 0;
}

float sample_1(unsigned int* seed)
{
    return 1;
}

float sample_few(unsigned int* seed)
{
    return random_to(1, 3, seed);
}

float sample_many(unsigned int* seed)
{
    return random_to(2, 10, seed);
}

int main()
{

    // Toy example
    // Declare variables in play
    float p_a, p_b, p_c;
    int n_threads = omp_get_max_threads();
    // printf("Max threads: %d\n", n_threads);
    // omp_set_num_threads(n_threads);
    float** dist_mixture = malloc(n_threads * sizeof(float*));
    split_array_allocate(dist_mixture, N, n_threads);

    // Initialize variables
    p_a = 0.8;
    p_b = 0.5;
    p_c = p_a * p_b;

    // Generate mixture
    int n_dists = 4;
    float weights[] = { 1 - p_c, p_c / 2, p_c / 4, p_c / 4 };
    float (*samplers[])(unsigned int*) = { sample_0, sample_1, sample_few, sample_many };

    mixture(samplers, weights, n_dists, dist_mixture, n_threads);
    printf("Sum(dist_mixture, N)/N = %f\n", split_array_sum(dist_mixture, N, n_threads) / N);
    // array_print(dist_mixture[0], N);
    split_array_free(dist_mixture, n_threads);

    return 0;
}
