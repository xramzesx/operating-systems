CFLAGS=-Wall -pthread -fsanitize=thread

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
	@gcc -c server.tmp.c -Wall && gcc server.tmp.o -o server.tmp.exe
	@gcc -c client.tmp.c -Wall && gcc client.tmp.o -o client.tmp.exe
	
clean:
	rm -f *.o *.so *.exe *.a 