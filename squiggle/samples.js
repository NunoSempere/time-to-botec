import { run } from "@quri/squiggle-lang";

let squiggle_code = `
p_a = 0.8
p_b = 0.5
p_c = p_a * p_b

dist_0 = 0
dist_1 = 1
dist_some = SampleSet.fromDist(1 to 3)
dist_many = SampleSet.fromDist(2 to 10)

dists = [dist_0, dist_1, dist_some, dist_many]
weights = [(1 - p_c), p_c/2, p_c/4, p_c/4 ]

result = mixture(dists, weights)
mean(result)

`

let output = run(squiggle_code, {
  defaultXYPointLength: 1000000,
  defaultSampleCount: 1000000,
  sparklineLength: 20,
})
console.log(output.result.value)
