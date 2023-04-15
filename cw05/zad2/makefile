CFLAGS=-Wall

#### COMPILATION ####

all:
	gcc -c main.c ${CFLAGS}
	gcc main.o -o main.exe

test:
	@make -s all
	@make -s test-single ACCURACY=0.001
	@make -s test-single ACCURACY=0.00001
	@make -s test-single ACCURACY=0.0000001
	@make -s test-single ACCURACY=0.000000001

test-single:
	@echo "Width\tForks\tTime\tResult"
	@./main.exe ${ACCURACY} 1
	@./main.exe ${ACCURACY} 5
	@./main.exe ${ACCURACY} 10
	@./main.exe ${ACCURACY} 20
	@./main.exe ${ACCURACY} 50
	@./main.exe ${ACCURACY} 101

test-save:
	@echo Test started
	@make -s test > raport.txt
	@echo Test finished

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a
