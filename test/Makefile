CC := gcc
CFLAG := -o3 -Wextra -Werror

all: main.c
	$(CC) $(CFLAG) main.c -o es.bin

run: all
	./es.bin

speed:
	wrk -d 2 -t 10 -c 100 http://localhost:8080