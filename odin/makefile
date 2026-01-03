MAKEFLAGS += --no-print-directory

install-odin:
	sudo mkdir -p /bin/odinlang
	sudo wget -P /bin/odinlang https://github.com/odin-lang/Odin/releases/download/dev-2025-12a/odin-linux-arm64-dev-2025-12a.tar.gz
	sudo tar -xf /bin/odinlang/odin-linux-arm64-dev-2025-12a.tar.gz -C /bin/odinlang/
	sudo rm /bin/odinlang/odin-linux-arm64-dev-2025-12a.tar.gz

build-c-like: 
	@odin build ./c-like/samples.odin -file -out:./c-like/samples -o:speed
build-st: 
	@odin build ./odin-like/samples-st.odin -file -out:./odin-like/samples-st -o:speed
build-mt: 
	@odin build ./odin-like/samples-mt.odin -file -out:./odin-like/samples-mt -o:speed
build-all:
	make build-c-like
	make build-st
	make build-mt

run-c-like: 
	@odin run ./c-like/samples.odin -file -out:./c-like/samples -o:speed
run-st: 
	@odin run ./odin-like/samples-st.odin -file -out:./odin-like/samples-st -o:speed
run-mt: 
	@odin run ./odin-like/samples-mt.odin -file -out:./odin-like/samples-mt -o:speed
run-all:
	make run-c-like
	make run-st
	make run-mt

profile-c-like:
	@make build-c-like
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do ./c-like/samples; done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time for c-like (Single Thread): |" | sed 's|$$|ms|' && echo

profile-st:
	@make build-st
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do ./odin-like/samples-st; done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time for idiomatic (Single Thread): |" | sed 's|$$|ms|' && echo
profile-mt:
	@make build-mt
	@t=$$(/usr/bin/time -f "%e" -p bash -c 'for i in {1..100}; do ./odin-like/samples-mt; done' 2>&1 >/dev/null | grep real | awk '{print $$2}' ); echo "scale=2; 1000 * $$t / 100" | bc | sed "s|^|Time for idiomatic (16 Threads): |" | sed 's|$$|ms|' && echo
profile-all:
	@make profile-c-like
	@make profile-st
	@make profile-mt