SHELL := /bin/bash ## <= required to use time

time-linux:
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	@echo "Running 100x and taking avg time of: $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do bun src/samples.js; done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time: |" | sed 's|$$|ms|' && echo

time-bun:
	time bun src/samples.js

run-bun:
	bun src/samples.js

run-node:
	node src/samples.js

