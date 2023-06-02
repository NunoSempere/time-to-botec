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
build: $(SRC)
	$(CC) $(OPTIMIZED) $(DEBUG) $(SRC) $(OPENMP) $(MATH) -o $(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) $(SRC_ONE_THREAD) $(OPENMP) $(MATH) -o $(OUTPUT_ONE_THREAD)

format: $(SRC)
	$(FORMATTER) $(SRC)

run: $(SRC) $(OUTPUT)
	OMP_NUM_THREADS=1 ./$(OUTPUT) && echo
	./$(OUTPUT_ONE_THREAD)

multi:
	OMP_NUM_THREADS=1 ./$(OUTPUT) && echo
	OMP_NUM_THREADS=2 ./$(OUTPUT) && echo
	OMP_NUM_THREADS=4 ./$(OUTPUT) && echo
	OMP_NUM_THREADS=8 ./$(OUTPUT) && echo
	OMP_NUM_THREADS=16 ./$(OUTPUT) && echo
	./$(OUTPUT_ONE_THREAD) && echo

time-linux: 
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	
	@echo "Running 100x and taking avg time: OMP_NUM_THREADS=1 $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do OMP_NUM_THREADS=1 $(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 1 thread: |" | sed 's|$$|ms|' && echo
	
	@echo "Running 100x and taking avg time: OMP_NUM_THREADS=2 $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do OMP_NUM_THREADS=2 $(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 2 threads: |" | sed 's|$$|ms|' && echo
	
	@echo "Running 100x and taking avg time: OMP_NUM_THREADS=4 $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do OMP_NUM_THREADS=4 $(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time for 4 threads: |" | sed 's|$$|ms|' && echo
	
	@echo "Running 100x and taking avg time: OMP_NUM_THREADS=8 $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do OMP_NUM_THREADS=8 $(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 8 threads: |" | sed 's|$$|ms|' && echo
	
	@echo "Running 100x and taking avg time: OMP_NUM_THREADS=16 $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do OMP_NUM_THREADS=16 $(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 16 threads: |" | sed 's|$$|ms|' && echo

time-linux-simple:
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	OMP_NUM_THREADS=1 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	OMP_NUM_THREADS=2 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	OMP_NUM_THREADS=4 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	OMP_NUM_THREADS=8 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	OMP_NUM_THREADS=16 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	/bin/time -f "Time: %es" ./$(OUTPUT_ONE_THREAD) && echo

debian-install-dependencies:
	sudo apt-get install libomp-dev

