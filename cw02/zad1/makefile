CFLAGS = -Wall
EX_FILE = file

#### COMPILATION ####

static-lib:
	gcc -c replace.c ${CFLAGS}
	ar rcs replace.a replace.o

main-sys:
	@make -s static-lib
	gcc -c main.c -D SYS ${CFLAGS}
	gcc main.o replace.a -o main_sys.exe ${CFLAGS}

main-lib:
	@make -s static-lib
	gcc -c main.c -D LIB ${CFLAGS}
	gcc main.o replace.a -o main_lib.exe ${CFLAGS}


#### MAIN ####

all:
	@make -s clean
	@make -s main-sys
	@make -s main-lib

#### COMPARE WITH TR ####

test:
	@make -s all
	
	./main_sys.exe a A ${EX_FILE}.txt ${EX_FILE}.sys.txt > 	pomiar_zad_1.txt
	./main_lib.exe a A ${EX_FILE}.txt ${EX_FILE}.lib.txt >> pomiar_zad_1.txt
	cat ${EX_FILE}.txt | tr a A > ${EX_FILE}.tmp.txt
	
	diff -s ${EX_FILE}.tmp.txt ${EX_FILE}.sys.txt
	diff -s ${EX_FILE}.tmp.txt ${EX_FILE}.lib.txt

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
	rm -f *.lib.txt *.sys.txt *.tmp.txt
