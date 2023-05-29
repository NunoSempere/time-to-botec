# Interface: 
#   make
#   make build
#   make format
#   make run

# Compiler
CC=gcc
# CC=tcc # <= faster compilation

# Main file
SRC=samples.c
OUTPUT=out/samples

## Dependencies
#  Has no dependencies
MATH=-lm

## Flags
DEBUG= #'-g'
STANDARD=-std=c99
WARNINGS=-Wall
OPTIMIZED=-O3 #-O3 actually gives better performance than -Ofast, at least for this version
OPENMP=-fopenmp

## Formatter
STYLE_BLUEPRINT=webkit
FORMATTER=clang-format -i -style=$(STYLE_BLUEPRINT)

## make build
build: $(SRC)
	$(CC) $(OPTIMIZED) $(DEBUG) $(SRC) $(OPENMP) $(MATH) -o $(OUTPUT)
	
#fast: Has been removed, compilation of "build" is very fast and it outputs optimized code by default

format: $(SRC)
	$(FORMATTER) $(SRC)

run: $(SRC) $(OUTPUT)
	OMP_NUM_THREADS=1 ./$(OUTPUT)

multi: $(SRC) $(OUTPUT)
	OMP_NUM_THREADS=1 ./$(OUTPUT) && echo
	OMP_NUM_THREADS=2 ./$(OUTPUT) && echo
	OMP_NUM_THREADS=4 ./$(OUTPUT)

time:
	OMP_NUM_THREADS=1 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	OMP_NUM_THREADS=2 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	OMP_NUM_THREADS=4 /bin/time -f "Time: %es" ./$(OUTPUT) && echo

linux-install:
	sudo apt-get install libomp-dev
