#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct xorshift32_state {
    uint32_t a;
};

uint32_t xorshift32(struct xorshift32_state *state)
{
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	uint32_t x = state->a;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return state->a = x;
}

float rand_xorshift32(struct xorshift32_state *state){
	return (float) xorshift32(state) / (float) UINT32_MAX;
}


int main(){
	struct xorshift32_state** states = malloc(sizeof(struct xorshift32_state*) * 4);
	for(int i=0; i<4;i++){
		states[i] = malloc(sizeof(struct xorshift32_state));
		states[i]->a = (uint32_t) i + 1;
	}

	for(int i=0; i<1000000000; i++){
		uint32_t x = xorshift32(states[0]);
		float y = rand_xorshift32(states[1]);
		// printf("%u\n", x);
		// printf("%f\n", y);
	}

	for(int i=0; i<4;i++){
		free(states[i]);
	}
	free(states);


  return 0;
}

