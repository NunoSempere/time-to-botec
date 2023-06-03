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

int main(){
	struct xorshift32_state** state = malloc(sizeof(struct xorshift32_state*) * 4);
	for(int i=0; i<4;i++){
		state[i] = malloc(sizeof(struct xorshift32_state));
		state[i]->a = (uint32_t) i + 1;
	}

	printf("%i\n", xorshift32(state[0]));
	printf("%i\n", xorshift32(state[0]));
	for(int i=0; i<4;i++){
		free(state[i]);
	}
	free(state);


  return 0;
}
