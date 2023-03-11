CFLAGS = -Wall

static-lib:
	gcc -c reverse.c ${CFLAGS}
	ar rcs reverse.a reverse.o

all:
	make static-lib
	gcc -c main.c ${CFLAGS}
	gcc main.o reverse.a -o main.exe ${CFLAGS}

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a

clean-all:
	make clean
	rm -f *.lib.txt *.sys.txt *.tmp.txt