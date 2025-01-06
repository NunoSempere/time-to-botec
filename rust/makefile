SHELL=bash
OUTPUT=./target/release/squiggle_rust

# why use a makefile? Because it abstracts away the individual build systems: npm/cargo/go/yarn/gcc/clang/etc.
# this reduces the cost of trying out new languages

build:
	cargo build --release
	$(OUTPUT)

time:
	time $(OUTPUT)

time-linux:
	@echo "Running 100x and taking avg time"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do OMP_NUM_THREADS=16 $(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time: |" | sed 's|$$|ms|' && echo

dev:
	cargo run

rustc: 
	rustc ./src/main.rs
	./main

