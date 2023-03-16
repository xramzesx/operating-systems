CFLAGS=-Wall

#### COMPILATION ####

all:
	gcc -c main.c ${CFLAGS}
	gcc main.o -o main.exe

test:
	@make -s all
	./main.exe 10

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
