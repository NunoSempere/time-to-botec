# Interface: 
#   make
#   make build
#   make format
#   make run

# Compiler
CC=gcc
# CC=tcc # <= faster compilation

# Main file

SRC_ONE_THREAD=./samples-one-thread.c
OUTPUT_ONE_THREAD=out/samples-one-thread

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
build: $(SRC_ONE_THREAD)
	mkdir -p out
	$(CC) $(OPTIMIZED) $(DEBUG) $(SRC_ONE_THREAD) $(OPENMP) $(MATH) -o $(OUTPUT_ONE_THREAD)

format: $(SRC_ONE_THREAD)
	$(FORMATTER) $(SRC_ONE_THREAD)

run: $(SRC_ONE_THREAD) $(OUTPUT_ONE_THREAD)
	./$(OUTPUT_ONE_THREAD)

time-linux: 
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	@echo "Running 100x and taking avg time: $(OUTPUT_ONE_THREAD)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do $(OUTPUT_ONE_THREAD); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time: |" | sed 's|$$|ms|' && echo

time-linux-simple:
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	/bin/time -f "Time: %es" ./$(OUTPUT_ONE_THREAD) && echo

debian-install-dependencies:
	sudo apt-get install libomp-dev

