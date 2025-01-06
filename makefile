SHELL := /bin/bash ## <= required to use time
MAKEFLAGS += --no-print-directory

build-all:
	cd C && make build
	cd nim && make fast
	cd ocaml && make fast
	cd squiggle.c && make build

time-all:
	@echo "# C" && cd C && make time-linux-fastest && echo && echo
	@echo "# squiggle.c" && cd squiggle.c && make build && make time-linux && echo && echo
	@echo "# squiggle.c (danger mode)" && cd squiggle.c && make build-experimental && make time-linux && echo && echo
	@echo "# squiggle.go" && cd go && make time-linux && echo && echo
	@echo "# rust" && cd rust && make time-linux && echo && echo
	@echo "# nim" && cd nim && make time-linux && echo && echo
	@echo "# lua (luajit)" && cd lua && make time-linux && echo && echo
	@echo "# Python (numpy)" && cd python && time python3 samples-fast.py && echo && echo
	@echo "# ocaml" && cd ocaml && make time-linux && echo && echo
	@echo "# fermi.go" && cd fermi.go && make time-linux && echo && echo
	@echo "# js (node)" && cd js && time node samples.js && echo && echo
	@echo "# js (bun)" && cd js && time bun samples.js && echo && echo
	@echo "# Squiggle (0.10.0)" && cd squiggle && make time-linux && echo && echo
	@echo "# SquigglePy (0.28)" && cd squigglepy && make time && echo && echo
	@echo "# R (4.2.1)" && cd R && time r samples.R && echo && echo
	@echo "# Python (3.12)" && cd python && time python3 samples.py && echo && echo
	@echo "# bc" && cd bc && make time && echo && echo

record:
	make time-all  2>&1 | tee time.txt
