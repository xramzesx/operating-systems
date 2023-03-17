CFLAGS=-Wall

#### COMPILATION ####
static-lib:
	gcc -c traverse.c ${CFLAGS}
	ar rcs traverse.a traverse.o

all:
	@make -s static-lib
	gcc -c main.c ${CFLAGS}
	gcc main.o traverse.a -o main.exe

test:
	@make -s clean
	@make -s all
	
	make -s test-dir DIRECTORY=.
	make -s test-dir DIRECTORY=..
	make -s test-dir DIRECTORY=../..
	make -s test-dir DIRECTORY=~
	make -s test-dir DIRECTORY=/root
	

test-dir:
	./main.exe ${DIRECTORY} \#include
	./main.exe ${DIRECTORY} asdf
	./main.exe ${DIRECTORY} zadanie

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
