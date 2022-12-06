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

## Performance table

With the [time](https://man7.org/linux/man-pages/man1/time.1.html) tool, using 1M samples:

| Language | Time      |
|----------|-----------|
| C        | 0m0,442s  |
| Node     | 0m0,732s  |
| Squiggle | 0m1,536s  |
| R        | 0m7,000s  |
| Python (CPython)  | 0m16,641s |


I was very surprised that Node/Squiggle code was almost as fast as the raw C code. For the Python code, it's possible that the lack of speed is more a function of me not being as familiar with Python. It's also very possible that the code would run faster with [PyPy](https://doc.pypy.org)
 
## Languages I may add later

- Julia (TuringML) 
- Rust
- Lisp
- ... and suggestions welcome
- Stan

## Roadmap

The future of this project is uncertain. In most words, I simply forget about this repository.

To do:
- [ ] Check whether the Squiggle code is producing 1M samples. Still not too sure.
- Differentiate between initial startup time (e.g., compiling, loading environment) and runtime. This matters because startup time could be ~constant, so for larger projects only the runtime matters.

## Other similar projects

- Squigglepy: <https://github.com/rethinkpriorities/squigglepy>
- Simple Squiggle: <https://github.com/quantified-uncertainty/simple-squiggle>
