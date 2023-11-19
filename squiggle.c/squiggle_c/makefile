MAKEFLAGS += --no-print-directory

## Formatter
STYLE_BLUEPRINT=webkit
FORMATTER=clang-format -i -style=$(STYLE_BLUEPRINT)

build-examples:
	cd examples/core && make all
	cd examples/more && make all

format-examples:
	cd examples/core && make format-all
	cd examples/more && make format-all

format: squiggle.c squiggle.h
	$(FORMATTER) squiggle.c
	$(FORMATTER) squiggle.h

lint:
	clang-tidy squiggle.c -- -lm
	clang-tidy extra.c -- -lm
