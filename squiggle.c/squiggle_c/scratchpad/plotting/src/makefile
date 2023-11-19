# Interface: 
#   make
#   make build
#   make format
#   make run

# Compiler
CC=gcc
# CC=tcc # <= faster compilation

# Main file
SRC=example.c ../../squiggle.c
OUTPUT=example

## Dependencies
MATH=-lm

## Flags
DEBUG= #'-g'
STANDARD=-std=c99
WARNINGS=-Wall
OPTIMIZED=-O3  #-Ofast
# OPENMP=-fopenmp

## Formatter
STYLE_BLUEPRINT=webkit
FORMATTER=clang-format -i -style=$(STYLE_BLUEPRINT)

## make build
build: $(SRC)
	$(CC) $(OPTIMIZED) $(DEBUG) $(SRC) $(MATH) -o $(OUTPUT)

format: $(SRC)
	$(FORMATTER) $(SRC)

run: $(SRC) $(OUTPUT)
	./$(OUTPUT) && echo

time-linux: 
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	
	@echo "Running 100x and taking avg time $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do ./$(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 1 thread: |" | sed 's|$$|ms|' && echo

## Profiling

profile-linux: 
	echo "Requires perf, which depends on the kernel version, and might be in linux-tools package or similar"
	echo "Must be run as sudo"
	$(CC) $(SRC) $(MATH) -o $(OUTPUT)
	sudo perf record ./$(OUTPUT)
	sudo perf report
	rm perf.data
