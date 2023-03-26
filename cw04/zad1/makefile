CFLAGS=-Wall

#### COMPILATION ####
static-lib:
	gcc -c signals.c ${CFLAGS}
	ar rcs signals.a signals.o

all:
	@make -s static-lib
	gcc -c main.c ${CFLAGS}
	gcc main.o signals.a -o main.exe
	
	
	gcc -c main_exec.c ${CFLAGS}
	gcc main_exec.o signals.a -o main_exec.exe

test:
	@make -s clean
	@make -s all

	@echo "=[fork only]=======\n"

	@./main.exe ignore
	@./main.exe handler
	@./main.exe mask
	@./main.exe pending

	@echo "\n=[exec]==========\n"

	@./main_exec.exe ignore
	@./main_exec.exe handler
	@./main_exec.exe mask
	@./main_exec.exe pending

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
