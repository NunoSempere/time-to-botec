Rust seems like it has a) great documentation, b) a better randomness generator than the one I was previously using.


The specific library I'll be using:

- https://crates.io/crates/rand_distr
- https://docs.rs/rand_distr/latest/rand_distr/index.html
- https://github.com/rust-random/rand/
- https://docs.rs/rand_distr/latest/rand_distr/index.html
- https://docs.rs/rand/latest/rand/

An underlying normal distribution algorithm that might be better than the Bo-Muller method.

- https://docs.rs/rand_distr/latest/src/rand_distr/normal.rs.html#238-307
- https://www.doornik.com/research/ziggurat.pdf
- https://en.wikipedia.org/wiki/Ziggurat_algorithm

A book produced as documentation (! <3%): https://rust-random.github.io/book/intro.html

----

Rust feels like pulling my teeth here, partly because I'm developing this on a server rather than on my own computer.

Some more ressources:
https://prng.di.unimi.it/#remarks
https://www.pcg-random.org/pdf/hmc-cs-2014-0905.pdf
https://web.archive.org/web/20160801142711/http://random.mat.sbg.ac.at/results/peter/A19final.pdf

