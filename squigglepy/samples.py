import squigglepy as sq
import numpy as np

p_a = 0.8
p_b = 0.5
p_c = p_a * p_b

dist_0 = 0
dist_1 = 1
dist_some = sq.to(1, 3)
dist_many = sq.to(2, 10)

dists = [dist_0, dist_1, dist_some, dist_many]
weights = [(1 - p_c), p_c/2, p_c/4, p_c/4 ]

result = sq.mixture(dists, weights)
result_samples = sq.sample(result, 1000000)
print(np.mean(result_samples))
