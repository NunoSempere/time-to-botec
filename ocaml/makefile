# Compiler
CC=ocamlopt
# ocamlopt: platform-specific, faster
# ocamlc: platform-independent intermediate representation, run with ocamlrun

# Main file
SRC=samples.ml
OUT=./out/samples

build: $(SRC)
	$(CC) $(SRC) -o $(OUT)
	mv samples.cmi samples.cmx samples.o ./out/

run:
	$(OUT)

time:
	bash -c "time $(OUT)"
