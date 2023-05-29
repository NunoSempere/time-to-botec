# C-Optimized

An optimized version of the original C implementation. 

The main changes are:

- an optimization of the mixture function (it passes the functions instead of the whole arrays, reducing in great measure the memory usage and the computation time) and 
- the implementation of multi-threading with OpenMP. 

## Performance

The mean time of execution is 6 ms. With the following distribution:

![Time histogram](https://i.imgur.com/6iT2PkF.png)

The hardware used has been an AMD 5800x3D and 16GB of DDR4-3200 MHz.

Also, the time data has been collected by executing the interior of the main() function 1000 times in a for loop, not executing the program itself 1000 times.

## Multithreading

Take into account that the multi-threading introduces a bit of dispersion in the execution time due to the creation and destruction of threads. 

In Nuño's machine, multithreading actually introduces a noticeable slowdown factor.
