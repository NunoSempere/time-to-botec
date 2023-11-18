build: 
	gcc xorshift-pointer.c -o out/xorshift-pointer
	gcc xorshift-struct.c -o out/xorshift-struct

run-pointer:
	./out/xorshift-pointer

run-struct:
	./out/xorshift-struct

time-pointer:
	/bin/time -f "Time: %es" ./out/xorshift-pointer && echo

time-struct:
	/bin/time -f "Time: %es" ./out/xorshift-struct && echo
