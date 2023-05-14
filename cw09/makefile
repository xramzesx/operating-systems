CC = gcc
CFLAGS = -Wall -pthread

main:
	@gcc -c main.c
	@gcc -o main.exe main.o $(CFLAGS)


all: main

test:
	@make -s clean
	@make -s all
	@./main.exe

clean:
	rm -f *.o *.so *.exe *.a