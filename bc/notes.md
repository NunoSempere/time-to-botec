## bc versions
https://git.gavinhoward.com/gavin/bc/src/branch/master
https://www.gnu.org/software/bc/manual/html_mono/bc.html
https://pubs.opengroup.org/onlinepubs/9699919799.2018edition/utilities/bc.html

## gh-bc

To build
./configure
make
sudo cp bin/bc /usr/bin/ghbc

Man, just feels nicer.
rand()
maxrand()

ghbc -l: include math functions, like log, sin 

## gnu bc

--standard: Process exactly the POSIX bc language.
Could define my own rng, and use arrays to do the seed thing

## Usage

Numbers are arbitrary precision numbers

length ( expression )
scale (expression)
scale=100
define t(x) {
  return(2);
}

Apparently posix bc only has one-letter functions, lol
Extensions needed: multi-letter functions 

## Decisions, decisions

Maybe target gh-bc, and then see about making it POSIX complicant later?

Decide between GH's bc, POSIX bc, and gnu bc
- Start with POSIX for now 
- Can't do POSIX, one letter functions are too annoying
