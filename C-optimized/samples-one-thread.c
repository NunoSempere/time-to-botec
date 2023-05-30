#include <math.h>
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

void array_fill(float* array, int length, float item)
{
    int i;
    {
        for (i = 0; i < length; i++) {
            array[i] = item;
        }
    }
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

float rand_float(float to)
{
    return ((float)rand() / (float)RAND_MAX) * to;
}

float ur_normal()
{
    float u1 = rand_float(1.0);
    float u2 = rand_float(1.0);
    float z = sqrtf(-2.0 * log(u1)) * sin(2 * PI * u2);
    return z;
}

inline float random_uniform(float from, float to)
{
    return ((float)rand() / (float)RAND_MAX) * (to - from) + from;
}

inline float random_normal(float mean, float sigma)
{
    return (mean + sigma * ur_normal());
}

inline float random_lognormal(float logmean, float logsigma)
{
    return expf(random_normal(logmean, logsigma));
}

inline float random_to(float low, float high)
{
    const float NORMAL95CONFIDENCE = 1.6448536269514722;
    float loglow = logf(low);
    float loghigh = logf(high);
    float logmean = (loglow + loghigh) / 2;
    float logsigma = (loghigh - loglow) / (2.0 * NORMAL95CONFIDENCE);
    return random_lognormal(logmean, logsigma);
}

void array_random_to(float* array, int length, float low, float high)
{
    int i;
    #pragma omp private(i)
    {
        #pragma omp for
        for (i = 0; i < length; i++) {
            array[i] = random_to(low, high);
        }
    }
}

void mixture(float (*samplers[])(void), float* weights, int n_dists, float* results, int results_length)
{
    float sum_weights = array_sum(weights, n_dists);
    float* normalized_weights = malloc(n_dists * sizeof(float));
    for (int i = 0; i < n_dists; i++) {
        normalized_weights[i] = weights[i] / sum_weights;
    }

    float* cummulative_weights = malloc(n_dists * sizeof(float));
    array_cumsum(normalized_weights, cummulative_weights, n_dists);

    //create var holders
    float p1;
    int sample_index, i, own_length;

    {
			for (int i = 0; i < results_length; i++) {
					p1 = random_uniform(0, 1);
					for (int j = 0; j < n_dists; j++) {
							if (p1 < cummulative_weights[j]) {
									results[i] = samplers[j]();
									break;
							}
					}
			}
    }
    free(normalized_weights);
    free(cummulative_weights);
}

float sample_0()
{
    return 0;
}

float sample_1()
{
    return 1;
}

float sample_few()
{
    return random_to(1, 3);
}

float sample_many()
{
    return random_to(2, 10);
}

int main()
{
    //initialize randomness
    srand(1);
    
		// clock_t start, end;
		// start = clock();

		// Toy example
		// Declare variables in play
		float p_a, p_b, p_c;
		// printf("Max threads: %d\n", n_threads);
		// omp_set_num_threads(n_threads);

		// Initialize variables
		p_a = 0.8;
		p_b = 0.5;
		p_c = p_a * p_b;

		// Generate mixture
		int n_dists = 4;
		float weights[] = { 1 - p_c, p_c / 2, p_c / 4, p_c / 4 };
		float (*samplers[])(void) = { sample_0, sample_1, sample_few, sample_many };

		float* results = malloc(N * sizeof(float));
		mixture(samplers, weights, n_dists, results, N);
		printf("Sum(dist_mixture, N)/N = %f\n", array_sum(results, N) / N);
		// array_print(dist_mixture[0], N);
		
		// end = clock();
		// printf("Time (ms): %f\n", ((double)(end - start)) / (CLOCKS_PER_SEC * 1000));
		// ^ Will only measure how long it takes the inner main to run, not the whole program, 
		// including e.g., loading the program into memory or smth.
		// Also CLOCKS_PER_SEC in POSIX is a constant equal to 1000000.
		// See: https://stackoverflow.com/questions/10455905/why-is-clocks-per-sec-not-the-actual-number-of-clocks-per-second
    return 0;
}
