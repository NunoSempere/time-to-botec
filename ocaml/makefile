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
