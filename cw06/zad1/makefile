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


#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a 