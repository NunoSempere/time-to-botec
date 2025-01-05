SHELL := /bin/bash

run:
	ghbc -l squiggle.bc estimate.bc

time:
	time ghbc -l squiggle.bc estimate.bc

get:
	git clone https://git.gavinhoward.com/gavin/bc.git && cd bc && ./configure && make && sudo cp bin/bc /usr/bin/bc

