import numpy as np

n = 1000 * 1000

def sample_beta_1_2():
    return np.random.beta(1,2)

a = np.array([sample_beta_1_2() for _ in range(n)])
print(np.mean(a))
