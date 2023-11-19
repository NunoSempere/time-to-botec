# imports
import numpy as np
rng = np.random.default_rng(123)
DEFAULT_N = 1000000

# three simple functions


def normal(mean, std, n=DEFAULT_N):
    return np.array(rng.normal(mean, std, n))


def lognormal(mean, std, n=DEFAULT_N):
    return np.array(rng.lognormal(mean, std, n))


def to(low, high, n=DEFAULT_N):
    normal95confidencePoint = 1.6448536269514722
    logLow = np.log(low)
    logHigh = np.log(high)
    meanlog = (logLow + logHigh)/2
    sdlog = (logHigh - logLow) / (2.0 * normal95confidencePoint)
    return lognormal(meanlog, sdlog, n)


def mixture(samples_list, weights_array, n=DEFAULT_N):
    normalized_weights = weights_array/sum(weights_array)
    cummulative_sums = np.cumsum(normalized_weights)
    helper_probs = rng.random(n)
    results = np.empty(n)
    for i in range(n):
        helper_list = [j for j in range(
            len(cummulative_sums)) if cummulative_sums[j] > helper_probs[i]]
        if len(helper_list) == 0:
            helper_loc = 0  # continue
            print("This should never happen")
        else:
            helper_loc = helper_list[0]
        target_samples = samples_list[helper_loc]
        result = rng.choice(target_samples, 1)[0]
        results[i] = result
    return (results)


# Example
p_a = 0.8
p_b = 0.5
p_c = p_a * p_b

dists = [[0], [1], to(1, 3), to(2, 10)]
# print(dists)
weights = np.array([1 - p_c, p_c/2, p_c/4, p_c/4])
# print(weights)
result = mixture(dists, weights)
mean_result = np.mean(result)
print(mean_result)
