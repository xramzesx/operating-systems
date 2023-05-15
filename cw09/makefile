CFLAGS = -Wall -pthread

#### COMPILATION ####

main:
	@gcc -c main.c
	@gcc -o main.exe main.o $(CFLAGS)


all: main

#### TEST ####

test:
	@make -s clean
	@make -s all
	@./main.exe

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a