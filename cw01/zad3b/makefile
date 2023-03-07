#### VARIABLES ####

CCFLAG = -Wall
FILE = instructions.txt

OPT = -O0

#### LIB COMPILATION ####

static-lib:
	gcc -c lib.c $(CCFLAG) $(OPT)
	ar rcs lib.a lib.o

shared-lib:
	gcc -fPIC -c lib.c $(CCFLAG) $(OPT)
	gcc -shared lib.o -o liblib.so

#### MAIN COMPILATION ####

main-static:
	make static-lib
	gcc -c main.c $(CCFLAG)
	gcc main.o lib.a -o main_static.exe $(CCFLAG) $(OPT)
	echo "./main_static.exe"

main-shared:
	make shared-lib
	gcc main.c -L./ -Wl,-rpath=./ -o main_shared.exe -llib $(CCFLAG) $(OPT)
	echo "./main_shared.exe"

main-dynamic:
	make shared-lib
	gcc main.c -D DLL -o main_dynamic.exe -ldl $(CCFLAG) $(OPT)
	echo "./main_dynamic.exe"

all:
	make main-static
	make main-shared
	make main-dynamic

#### TESTS ####

test-static:
	make main-static
	./main_static.exe < $(FILE) > results_$$(echo $(OPT) | cut -c 2-)_static.txt

test-shared:
	make main-shared
	./main_shared.exe < $(FILE) > results_$$(echo $(OPT) | cut -c 2-)_shared.txt

test-dynamic:
	make main-dynamic
	./main_dynamic.exe < $(FILE) > results_$$(echo $(OPT) | cut -c 2-)_dynamic.txt

### SINGLE TEST ###

test:
	make test-static
	make test-shared
	make test-dynamic

	make clean

### COMPLETE TEST ###

test-all:
	make test OPT=-O
	make test OPT=-O0
	make test OPT=-O1
	make test OPT=-O2
	make test OPT=-O3
	make test OPT=-Os

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
