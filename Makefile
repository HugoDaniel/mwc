CC = clang
CFLAGS = -Weverything -Wno-poison-system-directories -Wno-padded -Werror -std=c17 -ferror-limit=1
UBSAN = -fsanitize=undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion

build: tests
	@echo "\nInclude the mwc.h file in your project, it has no dependencies."

.cbuild/munit.o:
	@mkdir -p .cbuild
	@$(CC) -O3 -fsanitize=undefined -x c -c tests/munit.c -o .cbuild/munit.o

.cbuild/tests: mwc.h tests/mwc_tests.h .cbuild/munit.o
	@rm -f .cbuild/tests~
	@rm -f .cbuild/mwc_tests.o~
	@$(CC) -DMWC_IMPL -DMWC_TESTS -O $(CFLAGS) $(UBSAN) -Wno-unused-macros -Wno-unused-parameter -Wno-unused-variable -Wno-vla .cbuild/munit.o -x c mwc.h -o .cbuild/tests

tests: .cbuild/tests
	@./.cbuild/tests
	@rm -f .cbuild/tests~

clean_tests:
	@rm -f .cbuild/tests~
	@rm -f .cbuild/munit.o~

runtime_checks:
	@rm -f .cbuild/example
	@$(CC) -DMWC_IMPL -DMWC_EXAMPLE_USAGE -O2 $(CFLAGS) $(UBSAN) -Wno-unused-parameter -Wno-unused-variable -x c mwc.h -o .cbuild/example
	@ASAN_OPTIONS=detect_leaks=1 ./.cbuild/example

debug: 
	@rm -f .cbuild/tests~
	@rm -f .cbuild/mwc_tests.o~
	@rm -f .cbuild/munit.o~
	$(CC) -O0 -g -xc -c tests/munit.c -o .cbuild/munit.o
	$(CC) -DMWC_IMPL -DMWC_TESTS -O0 -g $(CFLAGS) -Wno-unused-parameter -Wno-unused-variable -Wno-vla -xc -c mwc.h -o .cbuild/mwc_tests.o
	$(CC) -DMWC_TESTS -O0 -g .cbuild/munit.o .cbuild/mwc_tests.o -o .cbuild/tests
	lldb .cbuild/tests

typecheck:
	@echo "Using frama-c, if this is not installed, please follow the instructions in https://frama-c.com"
	@opam exec frama-c -- -c11 -eva -eva-precision 1 mwc.h

clean:
	@rm -rf .cbuild/*
