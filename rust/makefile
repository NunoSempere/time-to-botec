SHELL=bash

# why use a makefile? Because it abstracts away the individual build systems: npm/cargo/go/yarn/gcc/clang/etc.
# this reduces the cost of trying out new languages

build:
	cargo build --release
	./target/release/squiggle_rust

time:
	time ./target/release/squiggle_rust

dev:
	cargo run

rustc: 
	rustc ./src/main.rs
	./main

