#### VARIABLES ####

CCFLAG = -Wall
FILE = instructions.txt

#### LIB COMPILATION ####

static-lib:
	gcc -c lib.c $(CCFLAG)
	ar rcs lib.a lib.o

shared-lib:
	gcc -fPIC -c lib.c $(CCFLAG)
	gcc -shared lib.o -o liblib.so

#### MAIN COMPILATION ####

main-static:
	make static-lib
	gcc -c main.c $(CCFLAG)
	gcc main.o lib.a -o main_static.exe $(CCFLAG)
	echo "./main_static.exe"

main-shared:
	make shared-lib
	gcc main.c -L./ -Wl,-rpath=./ -o main_shared.exe -llib $(CCFLAG)
	echo "./main_shared.exe"

main-dynamic:
	make shared-lib
	gcc main.c -D DLL -o main_dynamic.exe -ldl
	echo "./main_dynamic.exe"

all:
	make main-static
	make main-shared
	make main-dynamic

#### TESTS ####

test-static:
	make main-static
	./main_static.exe < $(FILE) > results_static.txt

test-shared:
	make main-shared
	./main_shared.exe < $(FILE) > results_shared.txt

test-dynamic:
	make main-dynamic
	./main_dynamic.exe < $(FILE) > results_dynamic.txt

test:
	make test-static
	make test-shared
	make test-dynamic
	make clean
#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
