build:
	go build main/fermi.go main/pretty.go main/error.go

run:
	go run main/fermi.go main/error.go main/pretty.go

install: fermi 
	rm -f /usr/bin/fermi
	rm -f /usr/bin/f
	sudo mv fermi /usr/bin/fermi
	sudo ln -s /usr/bin/fermi /usr/bin/f
