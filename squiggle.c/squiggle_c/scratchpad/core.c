
uint64_t xorshift64(uint64_t* seed)
{
    // Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
    // <https://en.wikipedia.org/wiki/Xorshift>
    uint64_t x = *seed;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return *seed = x;
}

double sample_unit_uniform(uint64_t* seed)
{
    // samples uniform from [0,1] interval.
    return ((double)xorshift64(seed)) / ((double)UINT64_MAX);
}

double sample_unit_normal(uint64_t* seed)
{
    // // See: <https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform>
    double u1 = sample_unit_uniform(seed);
    double u2 = sample_unit_uniform(seed);
    double z = sqrtf(-2.0 * log(u1)) * sin(2 * PI * u2);
    return z;
}

