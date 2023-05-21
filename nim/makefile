SHELL := /bin/bash ## <= required to use time
VERBOSE=--verbosity:0

build: samples.nim
	nim c $(VERBOSE) samples.nim

run: samples 
	./samples $(VERBOSE)

examine: samples
	nim c $(VERBOSE) -d:release samples.nim && time ./samples $(VERBOSE) && echo
	nim c $(VERBOSE) -d:danger samples.nim && time ./samples $(VERBOSE)
