# Time to BOTEC

## About

This repository contains example of very simple code to manipulate samples in various programming languages, and tallies their speed and the complexity of their code. It implements this platonic estimate:

```
p_a = 0.8
p_b = 0.5
p_c = p_a * p_b

dists = [0, 1, 1 to 3, 2 to 10]
weights = [(1 - p_c), p_c/2, p_c/4, p_c/4 ]

result = mixture(dists, weights) # should be 1M samples
mean(result)
```

I don't particularly care about the speed of this particular example. However, I think that how fast some approach is on this simple example might be indicative of that approach's speed when considering 100x or 1000x more complicated models. Having many different approaches computing the same model may also be useful for checking the validity of simple estimations. 

The name of this repository is a pun on two meanings of "time to": "how much time does it take to do x", and "let's do x". "BOTEC" stands for "back of the envelope calculation".

## Comparison table

| Language                    | Time      | Lines of code |
|-----------------------------|-----------|---------------|
| C (optimized, 16 threads)   | 5ms       | 249  |
| squiggle.c                  | 37ms      | 54*  | 
| Nim                         | 38ms      | 84   |
| Lua (LuaJIT)                | 68ms      | 82   |
| OCaml (flambda)             | 164ms     | 123  |
| Lua                         | 278ms     | 82   |
| C (naïve implementation)    | 292ms     | 149  |
| Javascript (NodeJS)         | 732ms     | 69   |
| Squiggle                    | 1,536s    | 14*  |
| SquigglePy                  | 1.602s    | 18*  |
| R                           | 7,000s    | 49   |
| Python (CPython)            | 16,641s   | 56   |

Time measurements taken with the [time](https://man7.org/linux/man-pages/man1/time.1.html) tool, using 1M samples. But different implementations use different algorithms and, occasionally, different time measuring methodologies (for the C, Nim and Lua implementations, I run the program 100 times and take the mean). Their speed was also measured under different loads in my machine. So I think that these time estimates are accurate within maybe ~2x or so.

Note that the number of lines is much shorter for Squiggle, SquigglePy and squiggle.c because I'm just counting the lines needed to get these libraries to output a model, not the lines inside them.

## Notes

### Nim

I was really happy trying [Nim](https://nim-lang.org/), and as a result the Nim code is a bit more optimized and engineered:

1. It is using the fastest "danger" compilation mode.
2. It has some optimizations: I don't compute 1M samples for each dist, but instead pass functions around and compute the 1M samples at the end
3. I define the normal function from scratch, using the [Box–Muller transform](https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform#Basic_form).
4. I also have a version in which I define the logarithm and sine functions themselves in nim to feed into the Box-Muller method. But it is much slower.

Without 1. and 2., the nim code takes 0m0.183s instead. But I don't think that these are unfair advantages: I liked trying out nim and therefore put in more love into the code, and this seems like it could be a recurring factor.

Ultimately, these optimizations were also incorporated into the C code as well.

Nim also has multithreading support, but I haven't bothered loooking into it yet.

### C

The optimizations which make the final C code significantly faster than the naïve implementation are:

- To pass around pointers to functions, instead of large arrays. This is the same as in the nim implementation, but imho leads to more complex code
- To use the Box-Muller transform instead of using libraries, like in nim.
- To use multithreading support

The C code uses the `-Ofast` or `-O3` compilation flags. Initially, without using those flags and without the algorithmic improvements, the code took ~0.4 seconds to run. So I was surprised that the Node and Squiggle code were comparable to the C code. It ended up being the case that the C code could be pushed to be ~100x faster, though :)

In fact, the C code ended up being so fast that I had to measure its time by running the code 100 times in a row and dividing that amount by 100, rather than by just running it once, because running it once was too fast for /bin/time. More sophisticated profiling tools exist that could e.g., account for how iddle a machine is when running the code, but I didn't think that was worth it at this point.

And still, there are some missing optimizations, like tweaking the code to take into account cache misses. I'm not exactly sure how that would go, though.

Once the code was at 6.6ms, there was a 0.6ms gain possible by using OMP better, and a 1ms gain by using a xorshift algorithm instead of rand_r from stdlib. I think there might be faster gains to be made by using OpenCL or CUDA, but I haven't gotten into how to do that instead.

Although the above paragraphs were written in the first person, the C code was written together with Jorge Sierra, who translated the algorithmic improvements from nim to it and added the initial multithreading support.

### squiggle.c

squiggle.c is a minimalistic library focused on understandability and being self-contained. It grew from the initial C code in this repository. You can see the code for the library [here](https://git.nunosempere.com/personal/squiggle.c), and the code for the example we are discussing [here](https://git.nunosempere.com/personal/squiggle.c/src/branch/master/examples/02_many_samples_time_to_botec).

### NodeJS and Squiggle

Using [bun](https://bun.sh/) instead of node is actually a bit slower. Also, both the NodeJS and the Squiggle code use [stdlib](https://stdlib.io/) in their innards, which has a bunch of interleaved functions that make the code slower. It's possible that not using that external library could make the code faster. But at the same time, the js approach does seem to be to use external libraries whenever possible.

I am not particularly sure that the Squiggle code is actually producing 1M samples, but I am also not in a rush to debug this.

### Python

For the Python code, it's possible that the lack of speed is more a function of me not being as familiar with Python. It's also very possible that the code would run faster with [PyPy](https://doc.pypy.org).

### R

R has a warm place in my heart from back in the day, and it has predefined functions to do everything. It was particularly fast to write for me, though not particularly fast to run :) R has some multithreading support, which I didn't use.

### Lua 

I was also really pleased with Lua. I liked the simplicity. And coming from javascript, I appreciated that the program did not fail silently, but rather gave me useful errors, like:

```
lua samples.lua
lua: samples.lua:42: table index is nil
stack traceback:
        samples.lua:42: in function 'mixture'
        samples.lua:49: in main chunk
        [C]: in ?
make: *** [makefile:14: run] Error 1
```

I also appreciated the speedup when using the LuaJIT interpreter. You can install LuaJIT with commands similar to:

```
git clone https://luajit.org/git/luajit.git
make
sudo make install
sudo ln -sf luajit-2.1.0-beta3 /usr/local/bin/luajit
```

Overall I'm thinking that a combination of lua at least for scripting and ¿nim/C/tbd? for more serious programs could be quite powerful.

### OCaml

OCaml was like meeting an old and forgotten friend. I found its syntax a bit clunky, but could get accustomed to it. Its list matching is nice, O(n) list element accessing, not so much. Honestly, I wanted to really like it, but I'm not sure yet. And it's *slow* compared to C.

I restricted myself to using the standard library, but it's likely that using Jane Street's Base or Core would have produced less clunky code.

### Overall thoughts

Overall I don't think that this is a fair comparison of the languages intrinsically, because I'm just differentially good at them, because I've chosen to put more effort in ones than in others. But it is still useful to me personally, and perhaps mildly informative to others. 

## Languages I may add later

- [ ] PyMC
- [ ] bc (the standard posix calculator)
- [ ] Zig
- [ ] Rust
- [x] OCaml
- [ ] Forth
- [ ] Julia (TuringML, MCHammer) 
- [ ] Lisp
- [ ] Go 
- [ ] sh/bash, lol?
- [ ] Dagger (<https://usedagger.com/>)
- [ ] Haskell
- [ ] OpenCL/CUDA (e.g., as in <https://www.eriksmistad.no/getting-started-with-opencl-and-gpu-computing/>). Seems like it would be overkill, and also the code would be way more complex. But maybe worth trying?
- ~~[ ] Stan => As far as I can tell, Stan is designed to generate samples from the posterior distribution given some data, not to create data by drawing samples from an arbitrary distribution.~~
  - [ ] Maybe still worth reversing the process?
- ... and suggestions welcome

## Roadmap

The future of this project is uncertain. In most words, I simply forget about this repository.

## Other similar projects

- Squigglepy: <https://github.com/rethinkpriorities/squigglepy>
- Simple Squiggle: <https://github.com/quantified-uncertainty/simple-squiggle>
