# Compiler
OC=ocamlopt
# ocamlopt: platform-specific, faster
# ocamlc: platform-independent intermediate representation, run with ocamlrun
FAST=-O3 -unsafe # install flambda with opam
PROF=-g

SRC=samples.ml
OUT=./out/samples

build: $(SRC)
	$(OC) $(SRC) -o $(OUT)
	mv samples.cmi samples.cmx samples.o ./out/

run:
	$(OUT)

fast:
	$(OC) $(FAST) $(SRC) -o $(OUT)
	mv samples.cmi samples.cmx samples.o ./out/

time:
	bash -c "time $(OUT)"

time-linux:
	@echo "Requires /bin/time, found on GNU/Linux systems" && echo
	@echo "Running 100x and taking avg time of: $(OUTPUT)"
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do ./out/samples; done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time: |" | sed 's|$$|ms|' && echo

profile:
	$(OC) $(PERF) $(SRC) -o $(OUT)
	mv samples.cmi samples.cmx samples.o ./out/
	sudo perf record -g $(OUT)
	sudo perf report
	rm perf.data

switch-opam-fast:
	opam switch create 4.11.2+flambda 
	eval $(opam env)

switch-opam-5.1.0:
	opam switch create 5.1.0 
	eval $(opam env)
