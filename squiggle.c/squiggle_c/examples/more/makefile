# Interface: 
# make all 
# make format-all
# make run-all
# make one             DIR=06_nuclear_recovery
# make format-one      DIR=06_nuclear_recovery
# make run-one         DIR=06_nuclear_recovery
# make time-linux-one  DIR=06_nuclear_recovery
# make profile-one     DIR=06_nuclear_recovery

# Compiler
CC=gcc
# CC=tcc # <= faster compilation

# Main file
SRC=example.c
OUTPUT=example

## Dependencies
SQUIGGLE=../../squiggle.c 
SQUIGGLE_MORE=../../squiggle_more.c
MATH=-lm
OPENMP=-fopenmp
DEPS=$(SQUIGGLE) $(SQUIGGLE_MORE) $(MATH) $(OPENMP)

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
	$(CC) $(OPTIMIZED) $(DEBUG) 00_example_template/$(SRC)             $(DEPS) -o 00_example_template/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 01_sample_from_cdf/$(SRC)              $(DEPS) -o 01_sample_from_cdf/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 02_sample_from_cdf_beta/$(SRC)         $(DEPS) -o 02_sample_from_cdf_beta/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 03_ci_beta/$(SRC)                      $(DEPS) -o 03_ci_beta/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 04_nuclear_war/$(SRC)                  $(DEPS) -o 04_nuclear_war/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 05_burn_10kg_fat/$(SRC)                $(DEPS) -o 05_burn_10kg_fat/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 06_nuclear_recovery/$(SRC)             $(DEPS) -o 06_nuclear_recovery/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 07_algebra/$(SRC)                      $(DEPS) -o 07_algebra/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 08_algebra_and_conversion/$(SRC)       $(DEPS) -o 08_algebra_and_conversion/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 09_ergonomic_algebra/$(SRC)            $(DEPS) -o 09_ergonomic_algebra/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 10_twitter_thread_example/$(SRC)       $(DEPS) -o 10_twitter_thread_example/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 11_billion_lognormals_paralell/$(SRC)  $(DEPS) -o 11_billion_lognormals_paralell/$(OUTPUT)
	$(CC) $(OPTIMIZED) $(DEBUG) 12_time_to_botec_parallel/$(SRC)       $(DEPS) -o 12_time_to_botec_parallel/$(OUTPUT)

format-all:
	$(FORMATTER) 00_example_template/$(SRC)
	$(FORMATTER) 01_sample_from_cdf/$(SRC)
	$(FORMATTER) 02_sample_from_cdf_beta/$(SRC)
	$(FORMATTER) 03_ci_beta/$(SRC)
	$(FORMATTER) 04_nuclear_war/$(SRC)
	$(FORMATTER) 05_burn_10kg_fat/$(SRC)
	$(FORMATTER) 06_nuclear_recovery/$(SRC)
	$(FORMATTER) 07_algebra/$(SRC)
	$(FORMATTER) 08_algebra_and_conversion/$(SRC)
	$(FORMATTER) 09_ergonomic_algebra/$(SRC)
	$(FORMATTER) 10_twitter_thread_example/$(SRC)
	$(FORMATTER) 11_billion_lognormals_paralell/$(SRC)
	$(FORMATTER) 12_time_to_botec_parallel/$(SRC)

run-all:
	00_example_template/$(OUTPUT)
	01_sample_from_cdf/$(OUTPUT)
	02_sample_from_cdf_beta/$(OUTPUT)
	03_ci_beta/$(OUTPUT)
	04_nuclear_war/$(OUTPUT)
	05_burn_10kg_fat/$(OUTPUT)
	06_nuclear_recovery/$(OUTPUT)
	07_algebra/$(OUTPUT)
	08_algebra_and_conversion/$(OUTPUT)
	09_ergonomic_algebra/$(OUTPUT)
	10_twitter_thread_example/$(OUTPUT)
	11_billion_lognormals_paralell/$(OUTPUT)
	12_time_to_botec_parallel/$(OUTPUT)

## make one DIR=06_nuclear_recovery
one: $(DIR)/$(SRC)
	$(CC) $(OPTIMIZED) $(DEBUG) $(DIR)/$(SRC) $(DEPS) -o $(DIR)/$(OUTPUT)

## make format-one DIR=06_nuclear_recovery
format-one: $(DIR)/$(SRC)
	$(FORMATTER) $(DIR)/$(SRC)

## make run-one DIR=06_nuclear_recovery
run-one: $(DIR)/$(OUTPUT)
	$(DIR)/$(OUTPUT) && echo

## make time-linux-one DIR=06_nuclear_recovery
time-linux-one: $(DIR)/$(OUTPUT)
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	@echo "Running 100x and taking avg time $(DIR)/$(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do $(DIR)/$(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time: |" | sed 's|$$|ms|' && echo

## e.g., make profile-linux-one DIR=06_nuclear_recovery
profile-linux-one: 
	echo "Requires perf, which depends on the kernel version, and might be in linux-tools package or similar"
	echo "Must be run as sudo"
	$(CC) $(OPTIMIZED) $(DEBUG) $(DIR)/$(SRC) $(DEPS) -o $(DIR)/$(OUTPUT)
	# $(CC) $(SRC) $(DEPS) -o $(OUTPUT)
	sudo perf record $(DIR)/$(OUTPUT)
	sudo perf report
	rm perf.data
