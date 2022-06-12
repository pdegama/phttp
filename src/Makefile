CC := gcc
CFLAGS :=-O3 -std=c99 -Wall -Wextra -Werror -g

PHTTP := build/phttp.o

all: build

build: clean make-dir build-phttp build-test
	echo "Build Successful"

make-dir:
	mkdir build
	echo "Make Build Dir..."

build-phttp: phttp.h phttp.c
	$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=199309L -c -o $(PHTTP) phttp.c
	echo "Build phttp successful"

build-test: test.c $(PHTTP)
	$(CC) $(CGLAGS) -o build/test.bin test.c $(PHTTP)
	echo "Build test successful"

run: build
	./build/test.bin

clean:
	rm -rf build
	echo "Clean Build Dir..."