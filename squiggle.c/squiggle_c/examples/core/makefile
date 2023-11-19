# Interface: 
# make all 
# make format-all
# make run-all
# make one             DIR=01_one_sample
# make format-one      DIR=01_one_sample
# make run-one         DIR=01_one_sample
# make time-linux-one  DIR=01_one_sample
# make profile-one     DIR=01_one_sample

# Compiler
CC=gcc
# CC=tcc # <= faster compilation

# Main file
SRC=example.c
OUTPUT=example

## Dependencies
SQUIGGLE=../../squiggle.c
MATH=-lm
DEPS=$(SQUIGGLE) $(MATH)

## Flags
DEBUG= #'-g'
STANDARD=-std=c99
WARNINGS=-Wall
OPTIMIZED=-O3  #-Ofast

## Formatter
STYLE_BLUEPRINT=webkit
FORMATTER=clang-format -i -style=$(STYLE_BLUEPRINT)

## make all
all:
	$(CC) $(OPTIMIZED) $(DEBUG) 00_example_template/$(SRC)    $(DEPS) -o 00_example_template/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 01_one_sample/$(SRC)          $(DEPS) -o 01_one_sample/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 02_time_to_botec/$(SRC)       $(DEPS) -o 02_time_to_botec/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 03_gcc_nested_function/$(SRC) $(DEPS) -o 03_gcc_nested_function/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 04_gamma_beta/$(SRC)          $(DEPS) -o 04_gamma_beta/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 05_hundred_lognormals/$(SRC)  $(DEPS) -o 05_hundred_lognormals/$(OUTPUT)

format-all:
	$(FORMATTER) 00_example_template/$(SRC)
	$(FORMATTER) 01_one_sample/$(SRC)
	$(FORMATTER) 02_time_to_botec/$(SRC)
	$(FORMATTER) 03_gcc_nested_function/$(SRC)
	$(FORMATTER) 04_gamma_beta/$(SRC)
	$(FORMATTER) 05_hundred_lognormals/$(SRC)

run-all:
	00_example_template/$(OUTPUT)
	01_one_sample/$(OUTPUT)
	02_time_to_botec/$(OUTPUT)
	03_gcc_nested_function/$(OUTPUT)
	04_gamma_beta/$(OUTPUT)
	05_hundred_lognormals/$(OUTPUT)
	
## make one DIR=01_one_sample
one: $(DIR)/$(SRC)
	$(CC) $(OPTIMIZED) $(DEBUG) $(DIR)/$(SRC) $(DEPS) -o $(DIR)/$(OUTPUT)

## make format-one DIR=01_one_sample
format-one: $(DIR)/$(SRC)
	$(FORMATTER) $(DIR)/$(SRC)

## make run-one DIR=01_one_sample
run-one: $(DIR)/$(OUTPUT)
	$(DIR)/$(OUTPUT) && echo

## make time-linux-one DIR=01_one_sample
time-linux-one: $(DIR)/$(OUTPUT)
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	@echo "Running 100x and taking avg time $(DIR)/$(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do $(DIR)/$(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 1 thread: |" | sed 's|$$|ms|' && echo

## e.g., make profile-linux-one DIR=01_one_sample
profile-linux-one: 
	echo "Requires perf, which depends on the kernel version, and might be in linux-tools package or similar"
	echo "Must be run as sudo"
	$(CC) $(OPTIMIZED) $(DEBUG) $(DIR)/$(SRC) $(DEPS) -o $(DIR)/$(OUTPUT)
	# $(CC) $(SRC) $(DEPS) -o $(OUTPUT)
	sudo perf record $(DIR)/$(OUTPUT)
	sudo perf report
	rm perf.data
