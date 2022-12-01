// Imports

import stdlib_normal from '@stdlib/random/base/normal/lib/main.js';
import stdlib_lognormal from '@stdlib/random/base/lognormal/lib/main.js';

// Functions
const DEFAULT_N = 10**6
function generator(f) {
  let g = (a, b, n = DEFAULT_N) => {
    let result = new Array(n)
    for (let i = 0; i < n; i++) {
      result[i] = f(a, b, n)
    }
    return result
  }
  return g
}
const sum = xs => xs.reduce((acc, x) => acc + x, 0)

const normal = generator(stdlib_normal)
const lognormal = generator(stdlib_lognormal)
const to = (low, high, n = DEFAULT_N) => {
  let normal95confidencePoint = 1.6448536269514722
  let logLow = Math.log(low)
  let logHigh = Math.log(high)
  let meanlog = (logLow + logHigh) / 2
  let sdlog = (logHigh - logLow) / (2.0 * normal95confidencePoint)
  return lognormal(meanlog, sdlog, n)
}

const mixture = (dists_array, weights_array, n = DEFAULT_N) => {
  let normalized_weights = weights_array.map(w => w / sum(weights_array))
  let cummulative_sums = Array(normalized_weights.length)
  normalized_weights.reduce((acc, x, i) => {
    cummulative_sums[i] = acc + x
    return cummulative_sums[i]
  }, 0)
  const helper_probs = [...new Array(n)].map(_ => Math.random())
  const results = helper_probs.map(p => {
    let match_index = cummulative_sums.findIndex(x => x > p)
    if(match_index == -1){
      console.log("Error: This should never happen.")
    }
    let target_loc = match_index // == -1 ? 0 : match_index
    let target_samples = dists_array[target_loc]
    return target_samples[Math.floor(Math.random() * target_samples.length)];
  })
  return(results)

}

// Example
let p_a = 0.8
let p_b = 0.5
let p_c = p_a * p_b

let dists = [
  [0], [1], to(1, 3),
  to(2, 10)
]
let weights = [
  (1 - p_c),
  p_c / 2,
  p_c / 4,
  p_c / 4
]
let result = mixture(dists, weights)
let mean_result = sum(result)/result.length
console.log(mean_result)
