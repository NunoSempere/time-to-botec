install:
	yarn install

run:
	node samples.js


time-linux: 
	@echo "Running 100x and taking avg time: ./squiggle"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {0..100}; do node samples.js; done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 16 threads: |" | sed 's|$$|ms|' && echo
