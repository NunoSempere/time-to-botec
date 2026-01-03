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
| Odin (Multi-Thread)         | 2.80ms    | 76   |
| C                           | 3.40ms    | 252  |
| squiggle.c (danger)         | 4.51ms    | 35*  | 
| squiggle.c                  | 6.80ms    | 31*  | 
| go                          | 12.30ms   | 141  | 
| Rust                        | 13.80ms   | 141  |
| Odin (Single-Thread)        | 20.00ms   | 39   | 
| Nim                         | 24.80ms   | 84   |
| Lua (LuaJIT)                | 53.70ms   | 82   |
| Python (numpy)              | 77ms      | 48   |
| OCaml (flambda)             | 96.30ms   | 123  |
| fermi.go                    | 139.00ms  | 17*  |
| Squiggle (bun)              | 236.20ms  | 14*  |
| Javascript (node)           | 281ms     | 69   |
| SquigglePy (v0.28)          | 940ms     | 18*  |
| R (4.2.1)                   | 4,826ms   | 49   |
| Python 3.12                 | 7,405ms   | 56   |
| Gavin Howard's bc           | 34,960ms  | 101  |

Time measurements taken with the [time](https://man7.org/linux/man-pages/man1/time.1.html) tool, using 1M samples. But different implementations use different algorithms and, occasionally, different time measuring methodologies (for the C, Odin, Nim and Lua implementations, I run the program 100 times and take the mean). Their speed was also measured under different loads in my machine. So I think that these time estimates are accurate within maybe ~2x or so.

Note that the number of lines is much shorter for Squiggle, SquigglePy and squiggle.c because I'm just counting the lines needed to get these libraries to output a model, not the lines inside them.

## Notes

### Odin [by [JJSierraM](https://github.com/JJSierraM)]

Odin was a discovery I made some time ago that made me really happy instantly after investigation. The idea behind the creator ([GingerBill](https://www.gingerbill.org/)¹) is to create a language that offers the benefits of C but with the ease of use and semantics of modern languages. Although it may sound like *yet another "C killer"*, it trully achieves its goals. It doesn´t have the verbosity and cumbersomeness of Rust (which, for me, is more a C++ substitute than C one) or the unfriendlyness that I find in Zig semantics. It manages to tickle a part of my brain that I very much enjoy being tickled. If you are a C enjoyer I definitely encourage you to try to learn Odin, reading through its [Overview](https://odin-lang.org/docs/overview/) or watching this (albeit a bit old) [presentation by its own creator](https://www.youtube.com/watch?v=rCqFdYUnC24). After this short discussion regarding the language, my comments on the implementation:

1. There are three versions of the code: c-like, odin-like/single-thread and odin-like/multi-thread
2. The C-like version is a literal translation from the C version named "02-better-algorithm-one-thread" into Odin.
3. The odin-like version are more Odin idiomatic, using all the capabilities from its libraries, reducing greatly the line count.
4. In the comparison table I have only included the single-thread and multi-thread idiomatic versions. The C-like version (which is single-thread) is slightly faster than the idiomatic one, but as I think the main beenfit of the language is its ergonomics while keeping the high performance, it was more adequate to include those versions. Also, it makes it easier to compare similar things.
5. If you don´t have Odin in your system, you can install it by calling make install-odin from the odin folder of the project (it requires sudo-ing as you have to save it in bin for the rest of the commands to work, as it assumes you have it in PATH)

The idiomatic, multithread vesion has achieved, in my machine², 2.80ms after executing it 100 times and averaging, being able to even beat the C multi threaded version, an absolute feat! I have developed it over a couple of days and as a first exercise to get to know the language. Even with my previous experience in high performance computing, I see this as an absolute victory for Odin. 

It is true that it uses the same approach as the Rust version (to compute and add up results instead of writing them in an array and then manipulating it), but as the Rust and Go versions do so and it's considered valid, I don´t see a problem with it.

___
¹ Careful with the flashbang from the link.

² My machine: AMD 5800X3D, 16GB DDR4 3200MHz.

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

squiggle.c is a minimalistic library focused on understandability and being self-contained. I've put a bunch of thought into how to design this in a way which is clean and fast. It grew from the initial C code in this repository. You can see the code for the library [here](https://git.nunosempere.com/personal/squiggle.c), which contains a thoughful README which I recommend people doing Monte Carlo estimation stuff read. 

I like the [operator](http://duskos.org/#operator) section of [Dusk OS](http://duskos.org/):

> Dusk OS doesn't have users, but operators. What's the difference? Control. You use a phone, you use a coffee machine, hell you even use a car these days. But you operate a bulldozer, you operate a crane, you operate a plane.

### Go

Go is reasonably fast, though not as fast as C. Partly this is because it uses a different, slightly more robust random number generator. I have high hopes for go, hopefully it will fit the role for me of a C with fewer warts.

### NodeJS and Squiggle

Using [bun](https://bun.sh/) instead of node is actually a bit slower for the raw js code. Also, both the NodeJS and the Squiggle code use [stdlib](https://stdlib.io/) in their innards, which has a bunch of interleaved functions that make the code slower. It's possible that not using that external library could make the code faster. But at the same time, the js approach does seem to be to use external libraries whenever possible. 

In general, Squiggle tries to present a simple interface to the user, leading to "hiding the magic" and having a bunch of [bugs](https://github.com/quantified-uncertainty/squiggle/labels/Bug), whereas I think the right tradeoff for me is to have some simple interface that I can operate skillfully (i.e., squiggle.c). 

### Python and Squigglepy

For the Python code, it's possible that the lack of speed is more a function of me not being as familiar with Python. It's also very possible that the code would run faster with [PyPy](https://doc.pypy.org). Update Feb/2024: Yeah, some well-tuned numpy code contributed by Jonathan Mann runs much faster.

In terms of complexity, SquigglePy seems to be between squiggle.c and the original squiggle. Like the original suqiggle, it also hides its stuff behind semi-magic wrappers, leading to e.g. ambiguities like around [correlated samples](https://git.nunosempere.com/personal/squiggle.c#correlated-samples) and generally having moving pieces that I don't understand. On the other hand, the code *is* short enough so that one person could read it in a few afternoons and roughly understand it. In terms of speed, SquigglePy seems slow.

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

### bc

The beautiful thing about bc is that it's an arbitrary precision calculator:

- it's not going to get floating point overflows, unlike practically everything else. Try `1000000001.0 ** 1000000.0` in OCaml, and you will get infinity, try p(1000000000.0, 1000000.0) and you will get a large power of 10 in bc.
- you can always trade get more precision (at the cost of longer running times). Could be useful if you were working with tricky long tails.

I decided to go with [Gavin Howard's bc](https://git.gavinhoward.com/gavin/bc), because I've been following the guy some time, and I respect him. It also had some crucial extensions, like a random number generator and allowing specifying functions and variables with names longer than one letter.

### Rust

I'm still making up my mind about Rust. It seems foreign. Mutable pointers seem kinda weird. Their randomness libraries are very nice, and the ziggurat method they're using to sample from a normal is very nice. I haven't added parallelism yet. You can see some notes in the Rust folder.

### Overall thoughts

Overall I don't think that this is a fair comparison of the languages intrinsically, because I'm just differentially good at them, because I've chosen to put more effort in ones than in others. But it is still useful to me personally, and perhaps mildly informative to others. 

## Languages I may add later

- [x] bc (the standard posix calculator)
- [x] OCaml
- [x] Go 
- [ ] Rust
- [ ] PyMC
- [ ] Zig
- [ ] Forth
- [ ] Julia (TuringML, MCHammer) 
- [ ] Lisp
- [ ] sh/bash, lol?
- [ ] Dagger (<https://usedagger.com/>)
- [ ] Haskell
- [ ] OpenCL/CUDA (e.g., as in <https://www.eriksmistad.no/getting-started-with-opencl-and-gpu-computing/>). Seems like it would be overkill, and also the code would be way more complex. But maybe worth trying?
- ~~[ ] Stan => As far as I can tell, Stan is designed to generate samples from the posterior distribution given some data, not to create data by drawing samples from an arbitrary distribution.~~
  - [ ] Maybe still worth reversing the process?
- ... and suggestions welcome

## Roadmap

The future of this project is uncertain. In most words, I simply forget about this repository.

TODO: Use [hyperfine](https://github.com/sharkdp/hyperfine) for time comparisons.

## Other similar projects

- Squigglepy: <https://github.com/rethinkpriorities/squigglepy>
- Simple Squiggle: <https://github.com/quantified-uncertainty/simple-squiggle>
