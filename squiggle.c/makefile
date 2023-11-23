OUTPUT=./samples

build:
	gcc -O3 samples.c ./squiggle_c/squiggle.c  ./squiggle_c/squiggle_more.c -lm -fopenmp -o $(OUTPUT)

install: 
	rm -r squiggle_c
	wget https://git.nunosempere.com/personal/squiggle.c/raw/branch/master/squiggle.c
	wget https://git.nunosempere.com/personal/squiggle.c/raw/branch/master/squiggle.h
	wget https://git.nunosempere.com/personal/squiggle.c/raw/branch/master/squiggle_more.c
	wget https://git.nunosempere.com/personal/squiggle.c/raw/branch/master/squiggle_more.h
	mkdir temp
	mv squiggle* temp
	mv temp squiggle_c
	wget https://git.nunosempere.com/personal/squiggle.c/raw/branch/master/examples/more/12_time_to_botec_parallel/example.c -O samples.c
	sed -i 's|../../..|squiggle_c|' samples.c

install-git:
	rm -r squiggle_c
	git clone https://git.nunosempere.com/personal/squiggle.c
	mv squiggle.c squiggle_c
	sudo rm -r squiggle_c/.git
	cp -r squiggle_c/examples/more/12_time_to_botec_parallel/example.c samples.c
	sed -i 's|../../..|squiggle_c|' samples.c

time-linux: 
	@echo "Running 100x and taking avg time: OMP_NUM_THREADS=16 $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do OMP_NUM_THREADS=16 $(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 16 threads: |" | sed 's|$$|ms|' && echo

install-small:
	rm -r squiggle_c
	git clone https://git.nunosempere.com/personal/squiggle.c
	mv squiggle.c squiggle_c
	sudo rm -r squiggle_c/.git
	cp -r squiggle_c/examples/core/02_time_to_botec/example.c samples.c
	sed -i 's|../../..|squiggle_c|' samples.c

build-small:
	gcc -O3 samples.c ./squiggle_c/squiggle.c  -lm -o $(OUTPUT)
