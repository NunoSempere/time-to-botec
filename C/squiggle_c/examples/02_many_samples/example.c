#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../squiggle.h"

// Estimate functions
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

int main(){
    // set randomness seed
		uint32_t* seed = malloc(sizeof(uint32_t));
		*seed = 1000; // xorshift can't start with 0

    float p_a = 0.8;
    float p_b = 0.5;
    float p_c = p_a * p_b;

    int n_dists = 4;
    float weights[] = { 1 - p_c, p_c / 2, p_c / 4, p_c / 4 };
    float (*samplers[])(uint32_t*) = { sample_0, sample_1, sample_few, sample_many };

		int n_samples = 1000000;
		float* result_many = (float *) malloc(n_samples * sizeof(float));
		for(int i=0; i<n_samples; i++){
      result_many[i] = mixture(samplers, weights, n_dists, seed);
		}
		
		printf("result_many: [");
		for(int i=0; i<100; i++){
		  printf("%.2f, ", result_many[i]);
		}
		printf("]\n");
}

/* 
Aggregation mechanisms:
- Quantiles (requires a sort)
- Sum 
- Average
- Std
*/
