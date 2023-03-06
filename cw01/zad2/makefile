static-lib:
	gcc -c lib.c -Wall
	ar rcs lib.a lib.o

shared-lib:
	gcc -fPIC -c lib.c -Wall
	gcc -shared lib.o -o lib.so

main-static:
	make static-lib
	gcc -c main_static.c -Wall
	gcc main_static.o lib.a -o main_static.exe -Wall
	./main_static.exe

