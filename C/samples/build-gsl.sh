#!/bin/bash
root="$1"
gcc -std=c99 -Wall -lm "$root".c -o "$root" -I/usr/local/include -L/usr/local/lib -lgsl -lgslcblas -lm -g

# Link libraries, for good measure
LD_LIBRARY_PATH=/usr/local/lib
export LD_LIBRARY_PATH

# Increase stack size limit
ulimit -Ss 256000
# -Ss: the soft limit. If you set the hard limit, you then can't raise it
# 256000: around 250Mbs, if I'm reading it correctly.

