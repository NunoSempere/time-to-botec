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

int main(){
	uint32_t** states = malloc(4 * sizeof(uint32_t*));
	for(int i=0; i<4;i++){
		states[i] = malloc(sizeof(uint32_t));
		*states[i] = i + 1;
	}

	printf("%i\n", xorshift32(states[0]));
	printf("%i\n", xorshift32(states[1]));
	for(int i=0; i<4;i++){
		free(states[i]);
	}
	free(states);

  return 0;
}
