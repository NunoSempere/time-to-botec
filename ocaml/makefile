# Compiler
CC=ocamlopt
# ocamlopt: platform-specific, faster
# ocamlc: platform-independent intermediate representation, run with ocamlrun

# Main file
SRC=samples.ml

build: $(SRC)
	$(CC) $(SRC) -o out/samples
	mv samples.cmi samples.cmx samples.o ./out/

run:
	./out/samples
