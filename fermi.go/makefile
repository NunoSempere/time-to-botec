install:
	git clone git@git.nunosempere.com:NunoSempere/fermi.git && cd fermi && make build && sudo make install
	yes | rm -rf fermi/.git
	cp fermi/more/time-to-botec.fermi samples.fermi

run:
	cat samples.fermi | fermi -n 1_000_000

time-linux:
	@echo "Running 100x and taking avg time: cat samples.fermi | fermi -n 1_000_000"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {0..100}; do cat samples.fermi | fermi -n 1_000_000; done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 16 threads: |" | sed 's|$$|ms|' && echo

