CFLAGS=-Wall

#### COMPILATION ####

all:
	gcc -c main.c ${CFLAGS}
	gcc main.o -o main.exe -lm
	gcc -c worker.c ${CFLAGS}
	gcc worker.o -o worker.exe

test:
	@make -s clean
	@make -s all
	@make -s test-single ACCURACY=0.001
	@make -s test-single ACCURACY=0.00001
	@make -s test-single ACCURACY=0.0000001
	@make -s test-single ACCURACY=0.000000001

test-single:
	@echo "Width\tForks\tTime\tResult"
	@make -s clean
	@make -s all
	@./main.exe ${ACCURACY} 1
	@./main.exe ${ACCURACY} 5
	@./main.exe ${ACCURACY} 10
	@./main.exe ${ACCURACY} 20
	@./main.exe ${ACCURACY} 50
	@./main.exe ${ACCURACY} 100

test-save:
	@echo Test started
	@make -s test > raport.txt
	@echo Test finished

#### UTILS ####

clean:
	rm -f *.o *.so *.exe *.a *.pipe
