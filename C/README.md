# Time to BOTEC in C

This repository contains a few implementations of a simple botec (back-of-the-envelope) calculation in C:

- In the folder C-01-simple/, you can see a simple implementation, which passes large arrays around
- In the folder C-02-better-algorithm-one-thread/ you can see a better implementations, that passes around pointers to functions, which makes the implementation more efficient
- The top level samples.c uses the algorithm improvements in C-02..., and also implements multithreading using OpenMP

## To do

- [ ] Add Windows/Powershell time-measuring commands
- [ ] Add CUDA?
- [x] Added results of perf. `rand_r` seems like a big chunk of it, but I'm hesitant to use lower-quality random numbers
  - [x] used xorshift instead
  - [-] Use xorshift with a struct instead of a pointer? idk, could be faster for some reason? => Tested, it takes the same time.
- [x] Update repository with correct timing
- [x] Use better profiling approach to capture timing with 1M samples.
- [x] See if program can be reworded so as to use multithreading effectively, e.g., so that you see speed gains proportional to the number of threads used
