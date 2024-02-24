import numpy as np
rng = np.random.default_rng(123)
DEFAULT_N = 1000000


def normal(mean, std, n=DEFAULT_N):
    return rng.normal(mean, std, n)


def lognormal(mean, std, n=DEFAULT_N):
    return rng.lognormal(mean, std, n)


def to(low, high, n=DEFAULT_N):
    normal95confidencePoint = 1.6448536269514722
    logLow = np.log(low)
    logHigh = np.log(high)
    meanlog = (logLow + logHigh) / 2
    sdlog = (logHigh - logLow) / (2 * normal95confidencePoint)
    return lognormal(meanlog, sdlog, n)


def optimized_mixture(samples_funcs, weights_array, n=DEFAULT_N):
    normalized_weights = weights_array / sum(weights_array)
    cummulative_sums = np.cumsum(normalized_weights)
    helper_probs = rng.random(n)
    results = np.empty(n)
    for i, (start, end) in enumerate(zip([0]+list(cummulative_sums[:-1]), cummulative_sums)):
        idx = np.where((helper_probs >= start) & (helper_probs < end))[0]
        # Generate only as many samples as needed for each distribution
        samples_func = samples_funcs[i]
        results[idx] = samples_func(n=len(idx))
    return results


p_a = 0.8
p_b = 0.5
p_c = p_a * p_b
dists = [
    lambda n=1: np.zeros(n),  # Distribution returning 0
    lambda n=1: np.ones(n),   # Distribution returning 1
    lambda n=1: to(1, 3, n),
    lambda n=1: to(2, 10, n)
]
weights = np.array([1 - p_c, p_c/2, p_c/4, p_c/4])
result = optimized_mixture(dists, weights)
mean_result = np.mean(result)
print(f'Mean result: {mean_result}')
