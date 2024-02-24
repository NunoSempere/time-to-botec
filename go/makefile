dev:
	go run squiggle.go

build:
	go build squiggle.go  

build-complex: 
	go build -ldflags="-s -w" squiggle.go  
	# https://stackoverflow.com/questions/45003259/passing-an-optimization-flag-to-a-go-compiler

build-show:
	go build -gcflags="-m" squiggle.go 
	# https://pkg.go.dev/cmd/compile
	# consider pgo: <https://go.dev/doc/pgo>

run:
	./squiggle

time-linux: 
	@echo "Running 100x and taking avg time: ./squiggle"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {0..100}; do ./squiggle; done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time using 16 threads: |" | sed 's|$$|ms|' && echo
