CFLAGS=-Wall

#### COMPILATION ####

static-lib:
	gcc -c shared.c $(CCFLAG)
	ar rcs shared.a shared.o

server:
	@make -s static-lib
	@gcc -c server.c $(CFLAGS) 
	@gcc server.o shared.a -o server.exe $(CFLAGS)


client:
	@make -s static-lib
	@gcc -c client.c $(CFLAGS) 
	@gcc client.o shared.a -o client.exe $(CFLAGS)

all:
	@make -s server
	@make -s client

#### UTILS ####

tmp:
	gcc -c reader.tmp.c -Wall &&  gcc reader.tmp.o -o reader.tmp.exe 
	gcc -c writer.tmp.c -Wall &&  gcc writer.tmp.o -o writer.tmp.exe 
	
clean:
	rm -f *.o *.so *.exe *.a 