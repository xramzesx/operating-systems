CFLAGS = -Wall

#### COMPILATION ####

static-lib:
	gcc -c dir.c ${CFLAGS}
	ar rcs dir.a dir.o

all:
	make static-lib
	gcc -c main.c ${CFLAGS}
	gcc main.o dir.a -o main.exe 

#### TESTS ####

test:
	make clean
	make all
	./main.exe

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
