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
OUTPUT=samples

## Dependencies
#  Has no dependencies

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
	$(CC) $(OPTIMIZED) $(DEBUG) $(OPENMP) $(SRC) -o $(OUTPUT)
	
#fast: Has been removed, compilation of "build" is very fast and it outputs optimized code by default

format: $(SRC)
	$(FORMATTER) $(SRC)

run: $(SRC) $(OUTPUT)
#	echo "Increasing stack size limit, because we are dealing with 1M samples"
#	# ulimit: increase stack size limit
#	# -Ss: the soft limit. If you set the hard limit, you then can't raise it
#	# 256000: around 250Mbs, if I'm reading it correctly.
#	# Then run the program
#	ulimit -Ss 256000 && ./$(OUTPUT)



# Old:
# Link libraries, for good measure
# LD_LIBRARY_PATH=/usr/local/lib
# export LD_LIBRARY_PATH

