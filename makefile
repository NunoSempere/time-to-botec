SHELL := /bin/bash ## <= required to use time
MAKEFLAGS += --no-print-directory

build-all:
	cd C && make build
	cd nim && make fast
	cd ocaml && make fast
	cd squiggle.c && make build

time-all:
	@echo "# bc" && cd bc && make time && echo && echo
	@echo "# C" && cd C && make time-linux-fastest && echo && echo
	@echo "# js (bun)" && cd js && time bun samples.js && echo && echo
	@echo "# js (node)" && cd js && time node samples.js && echo && echo
	@echo "# lua (luajit)" && cd lua && make time-linux && echo && echo
	@echo "# nim" && cd nim && make time-linux && echo && echo
	@echo "# ocaml" && cd ocaml && make time-linux && echo && echo
	@echo "# Python (3.9)" && cd python && time python3.9 samples.py && echo && echo
	@echo "# Python (numpy)" && cd python && time python3.9 samples-fast.py && echo && echo
	@echo "# R (3.6.1)" && cd R && time r samples.R && echo && echo
	@echo "# Squiggle (0.8.6)" && cd squiggle && make time-linux && echo && echo
	@echo "# SquigglePy (0.27)" && cd squigglepy && make time && echo && echo
	@echo "# squiggle.c" && cd squiggle.c && make install && make build && make time-linux && echo && echo
	@echo "# squiggle.c" && cd squiggle.c && make install-custom && make build-experimental &&  make time-linux && echo && echo
	@echo "# squiggle.c (danger mode)" && cd squiggle.c && make install-custom && make build-experimental && make time-linux && echo && echo
	@echo "# squiggle.go" && cd go && make time-linux && echo && echo

record:
	make time-all > time.txt 2>&1
