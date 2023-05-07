CFLAGS=-Wall

#### COMPILATION ####

linkedlist:
	gcc -c linkedlist.c $(CCFLAG)
	ar rcs linkedlist.a linkedlist.o


common:
	gcc -c common.c $(CCFLAG)
	ar rcs common.a common.o

static-lib:
	@make -s linkedlist
	@make -s common

barber:
	@make -s static-lib
	@gcc -c barber.c $(CFLAGS) 
	@gcc barber.o common.a linkedlist.a -o barber.exe $(CFLAGS)

client:
	@make -s static-lib
	@gcc -c client.c $(CFLAGS) 
	@gcc client.o common.a linkedlist.a -o client.exe $(CFLAGS)

main:
	@make -s static-lib
	@gcc -c main.c $(CFLAGS) 
	@gcc main.o common.a linkedlist.a -o main.exe $(CFLAGS)

all:
	@make -s barber
	@make -s client
	@make -s main

#### TEST ####

test:
	@make -s clean
	@make -s all
	./main.exe 13 10 10 25

test-inf:
	@make -s clean
	@make -s all
	./main.exe 13 10 10

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a 