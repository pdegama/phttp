CC := cc
CFLAGS :=-O3 -std=c99

all: clean build_core
	# Make Successful...

build_core: httpserver/httpserver.h httpserver/main.c
	mkdir lib
	mkdir build
	$(CC) $(CFLAGS) -g ./httpserver/main.c -o ./lib/httpc.bin

build_phttp:
	./lib/httpc

run_core:
	./lib/httpc.bin

clean:
	@rm -rf *.bin lib build *.o *.out
