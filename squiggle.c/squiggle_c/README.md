# squiggle.c

squiggle.c is a [grug-brained](https://grugbrain.dev/) self-contained C99 library that provides functions for simple Monte Carlo estimation, based on [Squiggle](https://www.squiggle-language.com/).

## Why C?

- Because it is fast
- Because I enjoy it
- Because C is honest
- Because it will last long
- Because it can fit in my head
- Because if you can implement something in C, you can implement it anywhere else
- Because it can be made faster if need be
  - e.g., with a multi-threading library like OpenMP, 
  - o  by implementing faster but more complex algorithms
  - or more simply, by inlining the sampling functions (adding an `inline` directive before their function declaration)
- **Because there are few abstractions between it and machine code** (C => assembly => machine code with gcc, or C => machine code, with tcc), leading to fewer errors beyond the programmer's control.

## Getting started

You can follow some example usage in the examples/ folder

1. In the [1st example](examples/01_one_sample/example.c), we define a small model, and draw one sample from it
2. In the [2nd example](examples/02_many_samples/example.c), we define a small model, and return many samples
3. In the [3rd example](examples/03_gcc_nested_function/example.c), we use a gcc extension—nested functions—to rewrite the code from point 2. in a more linear way.
4. In the [4th example](examples/04_sample_from_cdf_simple/example.c), we define some simple cdfs, and we draw samples from those cdfs. We see that this approach is slower than using the built-in samplers, e.g., the normal sampler.
5. In the [5th example](examples/05_sample_from_cdf_beta/example.c), we define the cdf for the beta distribution, and we draw samples from it. 
6. In the [6th example](examples/06_gamma_beta/example.c), we take samples from simple gamma and beta distributions, using the samplers provided by this library.
7. In the [7th example](examples/07_ci_beta/example.c), we get the 90% confidence interval of a beta distribution
8. The [8th example](examples/08_nuclear_war/example.c) translates the models from Eli and Nuño from [Samotsvety Nuclear Risk Forecasts — March 2022](https://forum.nunosempere.com/posts/KRFXjCqqfGQAYirm5/samotsvety-nuclear-risk-forecasts-march-2022#Nu_o_Sempere) into squiggle.c, then creates a mixture from both, and returns the mean probability of death per month and the 90% confidence interval.
8. The [9th example](examples/09_burn_10kg_fat/example.c) estimates how many minutes per day I would have to jump rope in order to lose 10kg of fat in half a year. 

## Commentary

### squiggle.c is short

[squiggle.c](squiggle.c) is less than 600 lines of C, with a core of <250 lines. The reader could just read it and grasp its contents.

### Core strategy

This library provides some basic building blocks. The recommended strategy is to:

1. Define sampler functions, which take a seed, and return 1 sample
2. Compose those sampler functions to define your estimation model
3. At the end, call the last sampler function many times to generate many samples from your model

### Cdf auxiliary functions


### Nested functions and compilation with tcc.

GCC has an extension which allows a program to define a function inside another function. This makes squiggle.c code more linear and nicer to read, at the cost of becoming dependent on GCC and hence sacrificing portability and increasing compilation times. Conversely, compiling with tcc (tiny c compiler) is almost instantaneous, but leads to longer execution times and doesn't allow for nested functions.

| GCC | tcc |
| --- | --- | 
| slower compilation | faster compilation | 
| allows nested functions | doesn't allow nested functions |
| faster execution | slower execution | 

My recommendation would be to use tcc while drawing a small number of samples for fast iteration, and then using gcc for the final version with lots of samples, and possibly with nested functions for ease of reading by others.

### Guarantees and licensing

- I offer no guarantees about stability, correctness, performance, etc. I might, for instance, abandon the version in C and rewrite it in Zig, Nim or Rust.
- This project mostly exists for my own usage & for my own amusement.
- Caution! Think carefully before using this project for anything important
- If you wanted to pay me to provide some stability or correctness, guarantees, or to tweak this library for your own usage, or to teach you how to use it, you could do so [here](https://nunosempere.com/consulting). Although this theoretical possibility exists, I don't I don't anticipate that this would be a good idea on most cases.

This project is released under the MIT license, a permissive open-source license. You can see it in the LICENSE.txt file. 

### Design choices

This code should aim to be correct, then simple, then fast.

- It should be correct. The user should be able to rely on it and not think about whether errors come from the library.
  - Nonetheless, the user should understand the limitations of sampling-based methods. See the section on [Tests and the long tail of the lognormal](https://git.nunosempere.com/personal/squiggle.c#tests-and-the-long-tail-of-the-lognormal) for a discussion of how sampling is bad at capturing some aspects of distributions with long tails.
- It should be clear, conceptually simple. Simple for me to implement, simple for others to understand.
- It should be fast. But when speed conflicts with simplicity, choose simplicity. For example, there might be several possible algorithms to sample a distribution, each of which is faster over part of the domain. In that case, it's conceptually simpler to just pick one algorithm, and pay the—normally small—performance penalty. In any case, though, the code should still be *way faster* than Python.

Note that being terse, or avoiding verbosity, is a non-goal. This is in part because of the constraints that C imposes. But it also aids with clarity and conceptual simplicity, as the issue of correlated samples illustrates in the next section.

### Correlated samples

In the original [squiggle](https://www.squiggle-language.com/) language, there is some ambiguity about what this code means:

```js
a = 1 to 10
b = 2 * a
c = b/a
c
```

Likewise in [squigglepy](https://github.com/rethinkpriorities/squigglepy):

```python
import squigglepy as sq
import numpy as np

a = sq.to(1, 3)
b = 2 * a  
c = b / a 

c_samples = sq.sample(c, 10)

print(c_samples)
```

Should `c` be equal to `2`? or should it be equal to 2 times the expected distribution of the ratio of two independent draws from a (`2 * a/a`, as it were)?

In squiggle.c, this ambiguity doesn't exist, at the cost of much greater overhead & verbosity:

```c
// correlated samples
// gcc -O3  correlated.c squiggle.c -lm -o correlated

#include "squiggle.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with a seed of 0

    double a = sample_to(1, 10, seed);
    double b = 2 * a;
    double c = b / a;

    printf("a: %f, b: %f, c: %f\n", a, b, c);
    // a: 0.607162, b: 1.214325, c: 0.500000

    free(seed);
}
```

vs

```c
// uncorrelated samples
// gcc -O3    uncorrelated.c ../../squiggle.c -lm -o uncorrelated

#include "squiggle.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

double draw_xyz(uint64_t* seed){
    // function could also be placed inside main with gcc nested functions extension.
    return sample_to(1, 20, seed);
}


int main(){
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with a seed of 0

    double a = draw_xyz(seed);
    double b = 2 * draw_xyz(seed);
    double c = b / a;

    printf("a: %f, b: %f, c: %f\n", a, b, c);
    // a: 0.522484, b: 10.283501, c: 19.681936

    free(seed)
}
```

### Tests and the long tail of the lognormal

Distribution functions can be tested with:

```sh
cd tests
make && make run
```

`make verify` is an alias that runs all the tests and just displays the ones that are failing. 

These tests are somewhat rudimentary: they get between 1M and 10M samples from a given sampling function, and check that their mean and standard deviations correspond to what they should theoretically should be.

If you run `make run` (or `make verify`), you will see errors such as these:

```
[-] Mean test for normal(47211.047473, 682197.019012) NOT passed.
Mean of normal(47211.047473, 682197.019012): 46933.673278, vs expected mean: 47211.047473
delta: -277.374195, relative delta: -0.005910

[-] Std test for lognormal(4.584666, 2.180816) NOT passed.
Std of lognormal(4.584666, 2.180816): 11443.588861, vs expected std: 11342.434900
delta: 101.153961, relative delta: 0.008839

[-] Std test for to(13839.861856, 897828.354318) NOT passed.
Std of to(13839.861856, 897828.354318): 495123.630575, vs expected std: 498075.002499
delta: -2951.371925, relative delta: -0.005961
```

These tests I wouldn't worry about. Due to luck of the draw, their relative error is a bit over 0.005, or 0.5%, and so the test fails. But it would surprise me if that had some meaningful practical implication.

The errors that should raise some worry are:

```
[-] Mean test for lognormal(1.210013, 4.766882) NOT passed.
Mean of lognormal(1.210013, 4.766882): 342337.257677, vs expected mean: 288253.061628
delta: 54084.196049, relative delta: 0.157985
[-] Std test for lognormal(1.210013, 4.766882) NOT passed.
Std of lognormal(1.210013, 4.766882): 208107782.972184, vs expected std: 24776840217.604111
delta: -24568732434.631927, relative delta: -118.057730

[-] Mean test for lognormal(-0.195240, 4.883106) NOT passed.
Mean of lognormal(-0.195240, 4.883106): 87151.733198, vs expected mean: 123886.818303
delta: -36735.085104, relative delta: -0.421507
[-] Std test for lognormal(-0.195240, 4.883106) NOT passed.
Std of lognormal(-0.195240, 4.883106): 33837426.331671, vs expected std: 18657000192.914921
delta: -18623162766.583248, relative delta: -550.371727

[-] Mean test for lognormal(0.644931, 4.795860) NOT passed.
Mean of lognormal(0.644931, 4.795860): 125053.904456, vs expected mean: 188163.894101
delta: -63109.989645, relative delta: -0.504662
[-] Std test for lognormal(0.644931, 4.795860) NOT passed.
Std of lognormal(0.644931, 4.795860): 39976300.711166, vs expected std: 18577298706.170452
delta: -18537322405.459286, relative delta: -463.707799
```

What is happening in this case is that you are taking a normal, like `normal(-0.195240, 4.883106)`, and you are exponentiating it to arrive at a lognormal. But `normal(-0.195240, 4.883106)` is going to have some noninsignificant weight on, say, 18. But `exp(18) = 39976300`, and points like it are going to end up a nontrivial amount to the analytical mean and standard deviation, even though they have little probability mass.

The reader can also check that for more plausible real-world values, like those fitting a lognormal to a really wide 90% confidence interval from 10 to 10k, errors aren't eggregious:

```
[x] Mean test for to(10.000000, 10000.000000) PASSED
[-] Std test for to(10.000000, 10000.000000) NOT passed.
Std of to(10.000000, 10000.000000): 23578.091775, vs expected std: 25836.381819
delta: -2258.290043, relative delta: -0.095779
```

Overall, I would caution that if you really care about the very far tails of distributions, you might want to instead use tools which can do some of the analytical manipulations for you, like the original Squiggle, Simple Squiggle (both linked below), or even doing lognormal multiplication by hand, relying on the fact that two lognormals multiplied together result in another lognormal with known shape.

In fact, squiggle.c does have a few functions for algebraic manipulations of simple distributions at the end of squiggle.c. But these are pretty rudimentary, and I don't know whether I'll end up expanding or deleting them.

### Results of running clang-tidy

clang-tidy is a utility to detect common errors in C/C++. You can run it with:

```
make tidy
```

It emits one warning about something I already took care of, so by default I've suppressed it. I think this is good news in terms of making me more confident that this simple library is correct :).

### Division between core functions and extraneous expansions

This library differentiates between core functions, which are pretty tightly scoped, and expansions and convenience functions, which are more meandering. Expansions are in `extra.c` and `extra.h`. To use them, take care to link them:

```
// In your C source file
#include "extra.h"
```

```
# When compiling:
gcc -std=c99 -Wall -O3 example.c squiggle.c extra.c -lm -o ./example

```

#### Extra: Cdf auxiliary functions

I provide some Take a cdf, and return a sample from the distribution produced by that cdf. This might make it easier to program models, at the cost of a 20x to 60x slowdown in the parts of the code that use it. 

#### Extra: Error propagation vs exiting on error

The process of taking a cdf and returning a sample might fail, e.g., it's a Newton method which might fail to converge because of cdf artifacts. The cdf itself might also fail, e.g., if a distribution only accepts a range of parameters, but is fed parameters outside that range.

This library provides two approaches:

1. Print the line and function in which the error occured, then exit on error
2. In situations where there might be an error, return a struct containing either the correct value or an error message:

```C
struct box {
    int empty;
    double content;
    char* error_msg;
};
```

The first approach produces terser programs but might not scale. The second approach seems like it could lead to more robust programmes, but is more verbose.

Behaviour on error can be toggled by the `EXIT_ON_ERROR` variable. This library also provides a convenient macro, `PROCESS_ERROR`, to make error handling in either case much terser—see the usage in example 4 in the examples/ folder.

Overall, I'd describe the error handling capabilities of this library as pretty rudimentary. For example, this program might fail in surprising ways if you ask for a lognormal with negative standard deviation, because I haven't added error checking for that case yet.


## Related projects

- [Squiggle](https://www.squiggle-language.com/)
- [SquigglePy](https://github.com/rethinkpriorities/squigglepy)
- [Simple Squiggle](https://nunosempere.com/blog/2022/04/17/simple-squiggle/)
- [time to botec](https://github.com/NunoSempere/time-to-botec)
- [beta]() 

## To do list

- [ ] Document paralellism
- [ ] Document confidence intervals
- [ ] Point out that, even though the C standard is ambiguous about this, this code assumes that doubles are 64 bit precision (otherwise the xorshift should be different).
- [ ] Document rudimentary algebra manipulations for normal/lognormal
- [ ] Think through whether to delete cdf => samples function
- [ ] Think through whether to:
  - simplify and just abort on error
  - complexify and use boxes for everything
  - leave as is
- [ ] Systematize references
- [ ] Support all distribution functions in <https://www.squiggle-language.com/docs/Api/Dist>
  - [ ] do so efficiently
- [ ] Add more functions to do algebra and get the 90% c.i. of normals, lognormals, betas, etc.
  - Think through which of these make sense.
- [ ] Disambiguate sample_laplace--successes vs failures || successes vs total trials as two distinct and differently named functions

## Done

- [x] Add example for only one sample
- [x] Add example for many samples
- [ ] ~~Add a custom preprocessor to allow simple nested functions that don't rely on local scope?~~
- [x] Use gcc extension to define functions nested inside main.
- [x] Chain various `sample_mixture` functions
- [x] Add beta distribution
  - See <https://stats.stackexchange.com/questions/502146/how-does-numpy-generate-samples-from-a-beta-distribution> for a faster method.
- [ ] ~~Use OpenMP for acceleration~~
- [x] Add function to get sample when given a cdf
- [x] Don't have a single header file.
- [x] Structure project a bit better
- [x] Simplify `PROCESS_ERROR` macro
- [x] Add README
  - [x] Schema: a function which takes a sample and manipulates it,
  - [x] and at the end, an array of samples.
  - [x] Explain boxes
  - [x] Explain nested functions
  - [x] Explain exit on error
  - [x] Explain individual examples
- [x] Rename functions to something more self-explanatory, e.g,. `sample_unit_normal`.
- [x] Add summarization functions: mean, std
- [x] Add sampling from a gamma distribution
  - https://dl.acm.org/doi/pdf/10.1145/358407.358414
- [x] Explain correlated samples
- [ ] ~~Add tests in Stan?~~
- [x] Test summary statistics for each of the distributions.
  - [x] For uniform
  - [x] For normal
  - [x] For lognormal
  - [x] For lognormal (to syntax)
  - [x] For beta distribution
- [x] Clarify gamma/standard gamma
- [x] Add efficient sampling from a beta distribution
  - https://dl.acm.org/doi/10.1145/358407.358414
  - https://link.springer.com/article/10.1007/bf02293108
  - https://stats.stackexchange.com/questions/502146/how-does-numpy-generate-samples-from-a-beta-distribution
  - https://github.com/numpy/numpy/blob/5cae51e794d69dd553104099305e9f92db237c53/numpy/random/src/distributions/distributions.c
- [x] Pontificate about lognormal tests
- [x] Give warning about sampling-based methods.
- [x] Have some more complicated & realistic example
- [x] Add summarization functions: 90% ci (or all c.i.?) 
- [x] Link to the examples in the examples section.
- [x] Add a few functions for doing simple algebra on normals, and lognormals
  - [x] Add prototypes
  - [x] Use named structs
  - [x] Add to header file
  - [x] Provide example algebra
  - [x] Add conversion between 90% ci and parameters.
  - [x] Use that conversion in conjuction with small algebra.
  - [x] Consider ergonomics of using ci instead of c_i
    - [x] use named struct instead
    - [x] demonstrate and document feeding a struct directly to a function; my_function((struct c_i){.low = 1, .high = 2});
  - [ ] Consider desirability of defining shortcuts for those functions. Adds a level of magic, though.
  - [ ] Test results
  - [x] Move to own file? Or signpost in file? => signposted in file.
- [x] Write twitter thread: now [here](https://twitter.com/NunoSempere/status/1707041153210564959); retweets appreciated.
- [ ] ~~Think about whether to write a simple version of this for [uxn](https://100r.co/site/uxn.html), a minimalistic portable programming stack which, sadly, doesn't have doubles (64 bit floats)~~
