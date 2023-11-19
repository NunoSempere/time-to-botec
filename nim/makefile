SHELL := /bin/bash ## <= required to use time
VERBOSE=--verbosity:0

fast: samples.nim
	nim c $(VERBOSE) -d:danger samples.nim

build: samples.nim
	nim c $(VERBOSE) samples.nim

run: samples 
	./samples $(VERBOSE)

examine: samples
	nim c $(VERBOSE) samples.nim && time ./samples $(VERBOSE) && echo
	nim c $(VERBOSE) -d:release samples.nim && time ./samples $(VERBOSE) && echo
	nim c $(VERBOSE) -d:danger samples.nim && time ./samples $(VERBOSE)

time-linux: 
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	@echo "Running 100x and taking avg time of: $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do ./samples; done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time: |" | sed 's|$$|ms|' && echo
