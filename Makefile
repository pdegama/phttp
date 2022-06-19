CC := gcc
CFLAGS :=-O3

all: clean build_core build_phttp test_build
	# Make Successful...

build_core: httpserver/httpserver.h httpserver
	mkdir lib
	mkdir build
	$(CC) $(CFLAGS) -g ./httpserver/main.c -o ./lib/httpc.bin
	# httpserver (core) build successful ...

build_phttp: src/phttp.h src
	$(CC) $(CFLAGS) -g -c ./src/phttp.c -o ./lib/phttp.o
	# phttp build successful ...

test_build:
	$(CC) $(CFLAGS) -g ./lib/phttp.o ./test/main.c -o ./build/test.bin
	$(CC) $(CFLAGS) -g ./lib/phttp.o ./test/long.c -o ./build/long.bin
	# Test build successful ...

run_core:
	./lib/httpc.bin

clean:
	@rm -rf *.bin lib build *.o *.out
