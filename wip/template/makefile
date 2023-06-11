# Interface: 
#   make
#   make build
#   make run
#   make time-linux
#   make install
#   make format
#   make time-linux-simple
#   make profile-linux

# Main file
SRC=samples.lang
OUTPUT=out/samples

# Compiler
COMPILER=lang-compiler
FLAGS=

## Formatter
FORMATTER=lang-formatter --options

## make build
build: $(SRC)
	$(COMPILER) $(FLAGS) -o $(OUTPUT)

install:
	lang-package-manager install dependencies

format: $(SRC)
	$(FORMATTER) $(SRC)

run: $(SRC) $(OUTPUT)
	./$(OUTPUT)

time-linux: 
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	@echo "Running 100x and taking avg time of: $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do $(OUTPUT); done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time: |" | sed 's|$$|ms|' && echo
	
time-linux-simple:
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	/bin/time -f "Time: %es" ./$(OUTPUT) && echo

profile-linux: 
	@echo "Requires perf, which depends on the kernel version, and might be in linux-tools package or similar"
	@echo "Must be run as sudo"
	sudo perf record $(OUTPUT)
	sudo perf report
	rm perf.data
