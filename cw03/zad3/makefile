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
	@make -s all
	
#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
