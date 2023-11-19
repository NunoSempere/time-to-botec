#include "../../../squiggle.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0
		double firstYearRussianNuclearWeapons = 1953;
		double	currentYear = 2023;

		for(int i=0; i<10; i++){
			double laplace_beta = sample_beta(currentYear-firstYearRussianNuclearWeapons + 1, 1, seed);
			printf("%f\n", laplace_beta);
		}
		
    free(seed);
}
