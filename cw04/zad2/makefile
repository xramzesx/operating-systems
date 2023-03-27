CFLAGS=-Wall

#### COMPILATION ####
static-lib:
	gcc -c signals.c ${CFLAGS}
	ar rcs signals.a signals.o

all:
	@make -s static-lib
	gcc -c main.c ${CFLAGS}
	gcc main.o signals.a -o main.exe

	# gcc -c main.c ${CFLAGS}
	# gcc main.o -o main.exe

#### TESTS ####

test:
	@make -s clean
	@make -s all

	### NAJLEPIEJ WYKONAĆ OSOBNO ###
	### Z KAŻDYM Z TYCH 3 PARAMETRÓW ###
	./main.exe siginfo
	./main.exe nodefer
	./main.exe resethand

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
