#include <math.h>
#include <stdint.h>
#include <stdlib.h>

const float PI = 3.14159265358979323846;

// Pseudo Random number generator

uint32_t xorshift32
(uint32_t* seed)
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

float rand_0_to_1(uint32_t* seed){
	return ((float) xorshift32(seed)) / ((float) UINT32_MAX);
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

// Array helpers
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

// Mixture function
float mixture(float (*samplers[])(uint32_t*), float* weights, int n_dists, uint32_t* seed)
{
    // You can see a simpler version of this function in the git history
    // or in C-02-better-algorithm-one-thread/
    float sum_weights = array_sum(weights, n_dists);
		float* cumsummed_normalized_weights = (float*) malloc(n_dists * sizeof(float));
		cumsummed_normalized_weights[0] = weights[0]/sum_weights;
    for (int i = 1; i < n_dists; i++) {
        cumsummed_normalized_weights[i] = cumsummed_normalized_weights[i - 1] + weights[i]/sum_weights;
    }

		float p = random_uniform(0, 1, seed);
		float result;
		for (int k = 0; k < n_dists; k++) {
				if (p < cumsummed_normalized_weights[k]) {
						result = samplers[k](seed);
						break;
				}
		}

		free(cumsummed_normalized_weights);
		return result;
}
