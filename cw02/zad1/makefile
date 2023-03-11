CFLAGS = -Wall

static-lib:
	gcc -c replace.c ${CFLAGS}
	ar rcs replace.a replace.o

main-sys:
	make static-lib
	gcc -c main.c -D SYS ${CFLAGS}
	gcc main.o replace.a -o main_sys.exe ${CFLAGS}

main-lib:
	make static-lib
	gcc -c main.c -D LIB ${CFLAGS}
	gcc main.o replace.a -o main_lib.exe ${CFLAGS}


all:
	make clean
	make main-sys
	make main-lib



clean:
	rm -f *.o *.so *.exe *.a