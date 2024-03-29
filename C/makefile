SHELL := /bin/bash ## <= required to use time
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
OPTIMIZED=-O3 #-O3 actually gives better performance than -Ofast, at least for this version. Could also add -march=native 
MORE_OPTIMIZATIONS=-Ofast -funit-at-a-time -march=native -fno-math-errno -ffast-math -std=gnu99 -fno-unroll-loops -flto
LOCAL=-march=native 
OPENMP=-fopenmp

## Formatter
STYLE_BLUEPRINT=webkit
FORMATTER=clang-format -i -style=$(STYLE_BLUEPRINT)


## make build
build: $(SRC)
	$(CC) $(OPTIMIZED) $(DEBUG) $(SRC) $(LOCAL) $(OPENMP) $(MATH) -o $(OUTPUT)

build-experimental:
	# https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
	rm -f *.gcda
	$(CC) -fprofile-generate $(OPTIMIZATIONS) $(LOCAL) samples.c $(OPENMP) $(MATH) -o $(OUTPUT)
	./$(OUTPUT)
	$(CC) -fprofile-use  $(OPTIMIZATIONS) $(LOCAL) samples.c $(OPENMP) $(MATH) -o $(OUTPUT)
	rm *.gcda
	# Using -Ofast increases speed a bit, but I don't trust it. <https://stackoverflow.com/questions/61232427/gcc-differences-between-o3-vs-ofast-optimizations>

static:
	$(CC) $(OPTIMIZED) $(DEBUG) $(SRC) $(LOCAL) $(OPENMP) $(MATH) -o $(OUTPUT)

format: $(SRC)
	$(FORMATTER) $(SRC)

run: $(SRC) $(OUTPUT)
	OMP_NUM_THREADS=1 ./$(OUTPUT) && echo

run-16: $(SRC) $(OUTPUT)
	OMP_NUM_THREADS=16 ./$(OUTPUT) && echo

multi:
	OMP_NUM_THREADS=1 ./$(OUTPUT) && echo
	OMP_NUM_THREADS=2 ./$(OUTPUT) && echo
	OMP_NUM_THREADS=4 ./$(OUTPUT) && echo
	OMP_NUM_THREADS=8 ./$(OUTPUT) && echo
	OMP_NUM_THREADS=16 ./$(OUTPUT) && echo

## Timing

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

time-linux-fastest: 
	@echo "Running 100x and taking avg time: OMP_NUM_THREADS=16 $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do OMP_NUM_THREADS=16 $(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 16 threads: |" | sed 's|$$|ms|' && echo

time-linux-simple:
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	OMP_NUM_THREADS=1 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	OMP_NUM_THREADS=2 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	OMP_NUM_THREADS=4 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	OMP_NUM_THREADS=8 /bin/time -f "Time: %es" ./$(OUTPUT) && echo
	OMP_NUM_THREADS=16 /bin/time -f "Time: %es" ./$(OUTPUT) && echo

## Profiling

profile-linux: 
	echo "Requires perf, which depends on the kernel version, and might be in linux-tools package or similar"
	echo "Must be run as sudo"
	$(CC) $(SRC) $(OPENMP) $(MATH) -o $(OUTPUT)
	# ./$(OUTPUT)
	# gprof: 
	# gprof $(OUTPUT) gmon.out > analysis.txt
	# rm gmon.out
	# vim analysis.txt
	# rm analysis.txt
	# perf: 
	OMP_NUM_THREADS=16 sudo perf record $(OUTPUT)
	sudo perf report
	rm perf.data
	

## Install
debian-install-dependencies:
	sudo apt-get install libomp-dev

