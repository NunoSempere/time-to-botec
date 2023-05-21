# Time to BOTEC

## About

This repository contains example of very simple code to manipulate samples in various programming languages. As of now, it may be useful for checking the validity of simple estimations.

The title of this repository is a pun on two meanings of "time to": "how much time does it take to do x", and "let's do x".

## Current languages

- [x] Python
- [x] R
- [x] Squiggle 
- [x] Javascript (NodeJS)
- [x] C
- [x] Nim 

## Performance table

With the [time](https://man7.org/linux/man-pages/man1/time.1.html) tool, using 1M samples:

| Language             | Time      |
|----------------------|-----------|
| Nim                  | 0m0.183s  |
| C                    | 0m0,442s  |
| Node                 | 0m0,732s  |
| Squiggle             | 0m1,536s  |
| R                    | 0m7,000s  |
| Python (CPython)     | 0m16,641s |

I was very surprised that Node/Squiggle code was almost as fast as the raw C code. For the Python code, it's possible that the lack of speed is more a function of me not being as familiar with Python. It's also very possible that the code would run faster with [PyPy](https://doc.pypy.org).

I was also really happy with trying [Nim](https://nim-lang.org/). The version which beats all others is just the normal usage of Nim, in the "release" compilation mode (the "danger" compilation mode shaves of a few more miliseconds). The Nim version has the particularity that I define the normal function from scratch, using the [Box–Muller transform](https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform#Basic_form). For Nim I also have a version of the code which takes around 4 seconds, where I define some very inefficient sine & logarithm functions to feed into the Box-Muller method, because it felt like fun to really write a botec tool really from scratch.

## Languages I may add later

- [ ] Julia (TuringML) 
- [ ] Rust
- [ ] Lisp
- [ ] Stan
- [ ] Go 
- [ ] Zig
- [ ] Forth
- ... and suggestions welcome

## Roadmap

The future of this project is uncertain. In most words, I simply forget about this repository.

To do:
- [ ] Check whether the Squiggle code is producing 1M samples. Still not too sure.
- Differentiate between initial startup time (e.g., compiling, loading environment) and runtime. This matters because startup time could be ~constant, so for larger projects only the runtime matters.

## Other similar projects

- Squigglepy: <https://github.com/rethinkpriorities/squigglepy>
- Simple Squiggle: <https://github.com/quantified-uncertainty/simple-squiggle>
