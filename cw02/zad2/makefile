CFLAGS = -Wall
CHUNK_SIZE = 1024
EX_FILE = example/file

static-lib:
	gcc -c reverse.c ${CFLAGS}
	ar rcs reverse.a reverse.o

main:
	echo ${CHUNK_SIZE}
	make static-lib
	gcc -c main.c ${CFLAGS} -D CHUNK_SIZE=${CHUNK_SIZE}
	gcc main.o reverse.a -o main_chunk_${CHUNK_SIZE}.exe 
	echo ${CHUNK_SIZE}

all:
	make main CHUNK_SIZE=1
	make main CHUNK_SIZE=1024

#### TESTS ####

test:
	make clean
	make all

	./main_chunk_1.exe 	  ${EX_FILE}.txt ${EX_FILE}.1.tmp.txt	 >  pomiar_zad_2.txt
	./main_chunk_1024.exe ${EX_FILE}.txt ${EX_FILE}.1024.tmp.txt >> pomiar_zad_2.txt

	./main_chunk_1.exe 	  ${EX_FILE}.1.tmp.txt	${EX_FILE}.1.rev.tmp.txt	
	./main_chunk_1024.exe ${EX_FILE}.1024.tmp.txt	${EX_FILE}.1024.rev.tmp.txt	 

	diff -s ${EX_FILE}.txt ${EX_FILE}.1.rev.tmp.txt
	diff -s ${EX_FILE}.txt ${EX_FILE}.1024.rev.tmp.txt
	

test-tac:
	make clean
	make all

	./main_chunk_1.exe 	  ${EX_FILE}.txt ${EX_FILE}.1.tmp.txt	 >  pomiar_zad_2.txt
	./main_chunk_1024.exe ${EX_FILE}.txt ${EX_FILE}.1024.tmp.txt >> pomiar_zad_2.txt

	tac ${EX_FILE}.txt | rev > ${EX_FILE}.rev.tmp.txt
	
	diff -s ${EX_FILE}.rev.tmp.txt ${EX_FILE}.1.tmp.txt
	diff -s ${EX_FILE}.rev.tmp.txt ${EX_FILE}.1024.tmp.txt


#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
	rm -f ${EX_FILE}*.tmp.txt
