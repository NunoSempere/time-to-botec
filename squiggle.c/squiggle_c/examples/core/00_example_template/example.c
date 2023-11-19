#include "../../../squiggle.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0

    // ...

    free(seed);
}
