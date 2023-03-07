static-lib:
	gcc -c lib.c -Wall
	ar rcs lib.a lib.o

shared-lib:
	gcc -fPIC -c lib.c -Wall
	gcc -shared lib.o -o lib.so

static-test:
	make static-lib
	gcc -c static-test.c -Wall
	gcc static-test.o lib.a -o static_test.exe -Wall
	./static_test.exe

all:
	make static-lib
	make shared-lib

clean:
	rm -f *.o *.so *.exe *.a
