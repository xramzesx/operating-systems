CFLAGS=-Wall

#### COMPILATION ####

static-lib:
	gcc -c common.c $(CCFLAG)
	ar rcs common.a common.o

barber:
	@make -s static-lib
	@gcc -c barber.c $(CFLAGS) 
	@gcc barber.o common.a -o barber.exe $(CFLAGS)

client:
	@make -s static-lib
	@gcc -c client.c $(CFLAGS) 
	@gcc client.o common.a -o client.exe $(CFLAGS)

main:
	@make -s static-lib
	@gcc -c main.c $(CFLAGS) 
	@gcc main.o common.a -o main.exe $(CFLAGS)

all:
	@make -s barber
	@make -s client
	@make -s main

#### UTILS ####

tmp:
	gcc -c reader.tmp.c -Wall &&  gcc reader.tmp.o -o reader.tmp.exe 
	gcc -c writer.tmp.c -Wall &&  gcc writer.tmp.o -o writer.tmp.exe 
	
clean:
	rm -f *.o *.so *.exe *.a 