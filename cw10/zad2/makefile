CFLAGS=-Wall -pthread

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

lall:
	@make -s server
	@make -s client

all:
	@make -s clean
	@make -C ../zad1 -s all
	@cp ../zad1/server.exe ../zad2/.
	@cp ../zad1/client.exe ../zad2/.

#### UTILS ####

tmp:
	@gcc -c server.tmp.c -Wall && gcc server.tmp.o -o server.tmp.exe
	@gcc -c client.tmp.c -Wall && gcc client.tmp.o -o client.tmp.exe
	
clean:
	rm -f *.o *.so *.exe *.a 

kill:
	chmod +x ./kill.sh
	./kill.sh
