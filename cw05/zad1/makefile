CFLAGS=-Wall

#### COMPILATION ####

static-lib:
	gcc -c mail.c $(CCFLAG)
	ar rcs mail.a mail.o

all:
	@make -s static-lib
	gcc -c main.c $(CCFLAG)
	gcc main.o mail.a -o main.exe $(CCFLAG)



test:
	@make -s all
	./main.exe sender

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
