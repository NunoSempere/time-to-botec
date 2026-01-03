package samples

import "core:math"
import "core:fmt"

PI :: 3.14159265358979323846
N :: 1000000
U32_MAX :: max(u32)

// Array helpers
//array_print s already natively implemented in Odin

array_sum :: proc (array : []f32) -> f32 {
    output : f32
    for item in array {
        output += item
    }
    return output
}

array_cumsum :: proc (array : []f32) -> []f32 {
    output := make([dynamic]f32, len(array))
    output[0] = array[0]
    for i in 1..<len(array) {
        output[i] = array[i] + output[i-1]
    }
    return output[:]
}

//Split array helpers

split_array_get_length :: proc (index, total_length, n_threads : int) -> int {
    return total_length % n_threads > index ? total_length / n_threads + 1 : total_length / n_threads
}

//TODO : multithread support

//TODO : SIMD support

// Pseudo Random number generator

xorshift32 :: proc (seed : ^u32) -> u32 {
    // Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
    // See <https://stackoverflow.com/questions/53886131/how-does-xorshift32-works>
    // https://en.wikipedia.org/wiki/Xorshift
    // Also some drama: <https://www.pcg-random.org/posts/on-vignas-pcg-critique.html>, <https://prng.di.unimi.it/>

    x :u32 = seed^
    x ~= x << 13 
    x ~= x >> 17 
    x ~= x << 5
    seed ^= x
    return x
}

// Distribution and sampling functions
rand_0_to_1 :: proc (seed : ^u32) -> f32 {
    return f32(xorshift32(seed)) / f32(U32_MAX)
}

rand_float :: proc (max : f32, seed : ^u32) -> f32 {
    return rand_0_to_1(seed) * max
}

ur_normal :: proc(seed : ^u32) -> f32 {
    u1, u2, z : f32
    u1 = rand_0_to_1(seed)
    u2 = rand_0_to_1(seed)
    z  = math.sqrt(-2.0 * math.ln(u1)) * math.sin(2 * PI * u2)
    return z
}

random_uniform :: proc (from, to : f32, seed : ^u32) -> f32 {
    return rand_0_to_1(seed) * (to - from) + from
}

random_normal :: proc (mean, sigma : f32, seed : ^u32) -> f32 {
    return mean + sigma * ur_normal(seed)
}

random_lognormal :: proc (logmean, logsigma : f32, seed : ^u32) -> f32 {
    return math.exp(random_normal(logmean, logsigma, seed))
}

random_to :: proc (low, high : f32, seed : ^u32) -> f32 {
    NORMAL95CONFIDENCE :: 1.6448536269514722
    loglow    : f32 = math.ln(low)
    loghigh   : f32 = math.ln(high)
    logmean   : f32 = (loglow + loghigh) / 2
    logsigman : f32 = (loghigh - loglow) / (2.0 * NORMAL95CONFIDENCE)
    return random_lognormal(logmean, logsigman, seed)
}

//TODO : mixture function
// Mixture function
mixture :: proc (samplers : []proc(^u32)->f32, weights, results : []f32) {
    n_dists : int = len(samplers)

    sum_weights : f32 = array_sum(weights)
    normalized_weights := make([dynamic]f32, n_dists)
    defer delete(normalized_weights)
    for i in 0..<n_dists {
        normalized_weights[i] = weights[i] / sum_weights
    }

    cummulative_weights := array_cumsum(normalized_weights[:])
    defer delete(cummulative_weights)

    seed : u32 = 1234 //Whatever you want EXCEPT 0
    for i in 0..<len(results) {
        p1 := random_uniform(0, 1, &seed)
        for weight, j in cummulative_weights {
            if p1 < weight {
                results[i] = samplers[j](&seed)
                break
            }
        }
    }
}

// Functions used for the BOTEC
// Their type has to be the same, as we will be passing them around

sample_0 :: proc (seed : ^u32) -> f32 {
    return 0.0
}

sample_1 :: proc (seed : ^u32) -> f32 {
    return 1.0
}

sample_few :: proc (seed : ^u32) -> f32 {
    return random_to(1, 3, seed)
}

sample_many :: proc (seed : ^u32) -> f32 {
    return random_to(2, 10, seed)
}

main :: proc() {
    //Declare variables
    p_a, p_b, p_c : f32

    // Initialize variables
    p_a, p_b = 0.8, 0.5
    p_c = p_a * p_b

    // Generate mixture
    weights := [?]f32{1 - p_c, p_c / 2, p_c / 4, p_c / 4}
    samplers := [?]proc(^u32)->f32{sample_0, sample_1, sample_few, sample_many}

    results := make([dynamic]f32, N)
    defer delete(results)
    mixture(samplers[:], weights[:], results[:])
    fmt.println("Sum(dist_mixture, N)/N = ", array_sum(results[:]) / N)
}