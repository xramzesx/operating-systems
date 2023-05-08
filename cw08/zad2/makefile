CC = gcc
CFLAGS = -Wall -pthread

all: life
	
life: grid.c life.c
	$(CC) $(CFLAGS) grid.c life.c -o life.exe -lncursesw

test:
	@make -s clean
	@make -s all
	./life.exe 13

clean:
	@rm -f life.exe
