#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t xorshift32(uint32_t* state)
{
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	uint32_t x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return *state = x;
}

float rand_xorshift32(uint32_t* state){
	return (float) xorshift32(state) / (float) UINT32_MAX;
}

int main(){
	uint32_t** states = malloc(4 * sizeof(uint32_t*));
	for(int i=0; i<4;i++){
		states[i] = malloc(sizeof(uint32_t));
		*states[i] = i + 1;
	}
  
	for(int i=0; i<100; i++){
		printf("%u\n", xorshift32(states[0]));
		printf("%f\n", rand_xorshift32(states[1]));
	}
	for(int i=0; i<4;i++){
		free(states[i]);
	}
	free(states);

  return 0;
}

// See <https://stackoverflow.com/questions/53886131/how-does-xorshift32-works>
// https://en.wikipedia.org/wiki/Xorshift
