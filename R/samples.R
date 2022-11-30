# Three simple functions
DEFAULT_N = 10000
normal <- function (mean, std, n=DEFAULT_N){
  return(rnorm(n, mean, std))
}

lognormal <- function(meanlog, sdlog, n=DEFAULT_N){
  return(rlnorm(n, meanlog = meanlog, sdlog = sdlog))
}

to <- function(low, high, n=DEFAULT_N){
  normal95confidencePoint = 1.6448536269514722
  logLow = log(low)
  logHigh = log(high)
  meanlog = (logLow + logHigh)/2
  sdlog = (logHigh - logLow) / (2.0 * normal95confidencePoint)
  return(lognormal(meanlog, sdlog, n))
}

mixture <- function(samples_list, weights_array, n=DEFAULT_N){ # note that this takes a list, not an array
  normalized_weights = weights_array/sum(weights_array)
  cummulative_sums = cumsum(normalized_weights)
  helper_probs = runif(n)
  results = vector(mode='numeric', length=n)
  for(i in c(1:n)){
    helper_which_list = which(cummulative_sums > helper_probs[i])
    helper_loc = ifelse(is.na(helper_which_list[1]), 1, helper_which_list[1])
    target_samples = samples_list[[helper_loc]]
    result = sample(target_samples, 1)
    results[i] = result
  }
  return(results)
}

# Example
p_a = 0.8
p_b = 0.5
p_c = p_a * p_b

dists = list(c(0), c(1), to(1, 3), to(2, 10))
# print(dists)
weights = c((1 - p_c), p_c/2, p_c/4, p_c/4)
# print(weights)
result = mixture(dists, weights)
mean_result = mean(result)
print(mean_result)



