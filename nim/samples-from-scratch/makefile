SHELL := /bin/bash

build: samples.nim
	nim c --verbosity:0 samples.nim

run: samples 
	./samples --verbosity:0

examine: samples
	# nim c --verbosity:0 --opt:speed -d:release -d:danger --checks:off samples.nim && time ./samples --verbosity:0 --checks:off
	nim c -d:release samples.nim && time ./samples
