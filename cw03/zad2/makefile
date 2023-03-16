CFLAGS=-Wall

#### COMPILATION ####

all:
	gcc -c main.c ${CFLAGS}
	gcc main.o -o main.exe

test:
	@make -s all
	./main.exe .
	./main.exe ./empty
	./main.exe /bin

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
