# Time to BOTEC

## About

This repository contains example of very simple code to manipulate samples in various programming languages. It implements this platonic estimate:

```
p_a = 0.8
p_b = 0.5
p_c = p_a * p_b

dists = [0, 1, 1 to 3, 2 to 10]
weights = [(1 - p_c), p_c/2, p_c/4, p_c/4 ]

result = mixture(dists, weights) # should be 1M samples
mean(result)
```

As of now, it may be useful for checking the validity of simple estimations. The title of this repository is a pun on two meanings of "time to": "how much time does it take to do x", and "let's do x".

## Current languages

- [x] C
- [x] Javascript (NodeJS)
- [x] Squiggle 
- [x] R
- [x] Python
- [x] Nim 

## Performance table

With the [time](https://man7.org/linux/man-pages/man1/time.1.html) tool, using 1M samples:

| Language             | Time      | Lines of code |
|----------------------|-----------|---------------|
| Nim                  | 0m0.068s  | 84  |
| C                    | 0m0.292s  | 149 |
| Javascript (NodeJS)  | 0m0,732s  | 69  |
| Squiggle             | 0m1,536s  | 14  |
| R                    | 0m7,000s  | 49  |
| Python (CPython)     | 0m16,641s | 56  |

## Notes

I was really happy trying [Nim](https://nim-lang.org/), and as a result the Nim code is a bit more optimized and engineered:

1. It is using the fastest "danger" compilation mode.
2. It has some optimizations: I don't compute 1M samples for each dist, but instead pass functions around and compute the 1M samples at the end
3. I define the normal function from scratch, using the [Box–Muller transform](https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform#Basic_form).
4. I also have a version in which I define the logarithm and sine functions themselves in nim to feed into the Box-Muller method. But it is much slower.

Without 1. and 2., the nim code takes 0m0.183s instead. But I don't think that these are unfair advantages: I liked trying out nim and therefore put in more love into the code, and this seems like it could be a recurring factor.

For C, I enabled the `-Ofast` compilation flag. Without it, it instead takes ~0.4 seconds. Initially, before I enabled the `-Ofast` flag, I was surprised that the Node and Squiggle code were comparable to the C code. Using [bun](https://bun.sh/) instead of node is actually a bit slower.

For the Python code, it's possible that the lack of speed is more a function of me not being as familiar with Python. It's also very possible that the code would run faster with [PyPy](https://doc.pypy.org).

R has a warm place in my heart from back in the day, and it has predefined functions to do everything. It was particularly fast to write for me, though not particularly fast to run :)

Overall I don't think that this is a fair comparison of the languages intrinsically, because I'm just differentially good at them, because I've chosen to put more effort in ones than in others. But it is still useful to me personally, and perhaps mildly informative to others. 

## Languages I may add later

- [ ] Julia (TuringML) 
- [ ] Rust
- [ ] Lisp
- [ ] Stan
- [ ] Go 
- [ ] Zig
- [ ] Forth
- [ ] OCaml
- [ ] Haskell
- [ ] CUDA
- ... and suggestions welcome

## Roadmap

The future of this project is uncertain. In most words, I simply forget about this repository.

To do:
- [ ] Check whether the Squiggle code is producing 1M samples. Still not too sure.
- [-] Differentiate between initial startup time (e.g., compiling, loading environment) and runtime. This matters because startup time could be ~constant, so for larger projects only the runtime matters. Particularly for Julia. <= nah, too difficult.

## Other similar projects

- Squigglepy: <https://github.com/rethinkpriorities/squigglepy>
- Simple Squiggle: <https://github.com/quantified-uncertainty/simple-squiggle>
