CFLAGS=-Wall

#### COMPILATION ####

all:
	gcc -c main.c ${CFLAGS}
	gcc main.o -o main.exe

test:
	@make -s all
	./main.exe 0.000000001 1
	./main.exe 0.000000001 5
	./main.exe 0.000000001 10
	./main.exe 0.000000001 20
	./main.exe 0.000000001 50
	./main.exe 0.000000001 101

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
