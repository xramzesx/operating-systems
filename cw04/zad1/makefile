CFLAGS=-Wall

#### COMPILATION ####
static-lib:
	gcc -c signals.c ${CFLAGS}
	ar rcs signals.a signals.o

all:
	@make -s static-lib
	gcc -c main.c ${CFLAGS}
	gcc main.o signals.a -o main.exe
	

test:
	@make -s clean
	@make -s all

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
