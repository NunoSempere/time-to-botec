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
DEPS='gsl'

## Flags
INCS=`pkg-config --cflags ${DEPS}`
LIBS=`pkg-config --libs ${DEPS}`
DEBUG= #'-g'
STANDARD=-std=c99
WARNINGS=-Wall
FAST=-Ofast
## Formatter
STYLE_BLUEPRINT=webkit
FORMATTER=clang-format -i -style=$(STYLE_BLUEPRINT)

## make build
build: $(SRC)
	$(CC) $(DEBUG) $(INCS) $(PLUGS) $(SRC) -o samples $(LIBS)

fast: $(SRC)
	$(CC) $(FAST) $(DEBUG) $(INCS) $(PLUGS) $(SRC) -o samples $(LIBS)

format: $(SRC)
	$(FORMATTER) $(SRC)

run: $(SRC) $(OUTPUT)
	echo "Increasing stack size limit, because we are dealing with 1M samples"
	# ulimit: increase stack size limit
	# -Ss: the soft limit. If you set the hard limit, you then can't raise it
	# 256000: around 250Mbs, if I'm reading it correctly.
	# Then run the program
	ulimit -Ss 256000 && ./$(OUTPUT)



# Old:
# Link libraries, for good measure
# LD_LIBRARY_PATH=/usr/local/lib
# export LD_LIBRARY_PATH

