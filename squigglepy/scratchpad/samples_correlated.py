import squigglepy as sq
import numpy as np

a = sq.to(1, 3)
b = a / 2 
c = b / a 

c_samples = sq.sample(c, 10)

print(c_samples)
