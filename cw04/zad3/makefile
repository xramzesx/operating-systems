CFLAGS=-Wall

#### COMPILATION ####

all:	
	gcc -c catcher.c ${CFLAGS}
	gcc catcher.o -o catcher.exe

	gcc -c sender.c ${CFLAGS}
	gcc sender.o -o sender.exe
	

test:
	@make -s clean
	@make -s all


kill:
	kill -SIGSTOP $(pidof ./sender.exe) 
	kill -SIGSTOP $(pidof ./catcher.exe)

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
