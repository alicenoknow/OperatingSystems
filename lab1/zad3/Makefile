CC = gcc -Wall

CC0 = gcc -Wall -O0
CC2 = gcc -Wall -O2
CCs = gcc -Wall -Os

static:
	make clean
	$(CC) -c firstlib.c
	ar crs libfirstlib.a firstlib.o
	$(CC) test3.c -o test3 -L. -lfirstlib
	@echo 'STATIC LIBLARY\n' >> raport3a.txt

shared:
	make clean
	$(CC) -c -fPIC firstlib.c
	$(CC) -shared -o libfirstlib.so firstlib.o
	$(CC) -L. -o test3 test3.c -lfirstlib -Wl,-rpath=`pwd`
	@echo 'SHARED LIBLARY\n' >> raport3a.txt


dynamic:
	make clean
	$(CC) -fPIC -shared -o libfirstlib.so firstlib.c
	$(CC) -o test3 dynamic_test.c -ldl
	@echo 'DYNAMIC LIBLARY\n' >> raport3a.txt


clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *.out
	rm -f test3
	rm -f tmp*.txt
	
runA:
	make static
	./test3 raport3a.txt
	make shared
	./test3 raport3a.txt
	make dynamic
	LD_LIBRARY_PATH=/home/alice/Desktop/SysOpy/lab1/zad3 ./test3 raport3a.txt
	make clean
	

runB:
	
	make clean
	@echo 'NO OPTIMIZATION \n' >> raport3b.txt
	$(CC) -c firstlib.c
	ar crs libfirstlib.a firstlib.o
	$(CC) test3.c -o test3 -L. -lfirstlib
	@echo 'STATIC LIBLARY\n' >> raport3b.txt
	./test3 raport3b.txt
	
	make clean
	@echo 'OPTIMIZATION -O0\n' >> raport3b.txt
	$(CC0) -c firstlib.c
	ar crs libfirstlib.a firstlib.o
	$(CC0) test3.c -o test3 -L. -lfirstlib
	@echo 'STATIC LIBLARY\n' >> raport3b.txt
	./test3 raport3b.txt
	
	make clean
	@echo 'OPTIMIZATION -O2\n' >> raport3b.txt
	$(CC2) -c firstlib.c
	ar crs libfirstlib.a firstlib.o
	$(CC2) test3.c -o test3 -L. -lfirstlib
	@echo 'STATIC LIBLARY\n' >> raport3b.txt
	./test3 raport3b.txt
	
	make clean
	@echo 'OPTIMIZATION -Os\n' >> raport3b.txt
	$(CCs) -c firstlib.c
	ar crs libfirstlib.a firstlib.o
	$(CCs) test3.c -o test3 -L. -lfirstlib
	@echo 'STATIC LIBLARY\n' >> raport3b.txt	
	./test3 raport3b.txt
	
	make clean
	@echo 'NO OPTIMIZATION\n' >> raport3b.txt
	$(CC) -c -fPIC firstlib.c
	$(CC) -shared -o libfirstlib.so firstlib.o
	$(CC) -L. -o test3 test3.c -lfirstlib -Wl,-rpath=`pwd`
	@echo 'SHARED LIBLARY\n' >> raport3b.txt
	./test3 raport3b.txt
	
	make clean
	@echo 'OPTIMIZATION -O0\n' >> raport3b.txt
	$(CC0) -c -fPIC firstlib.c
	$(CC0) -shared -o libfirstlib.so firstlib.o
	$(CC0) -L. -o test3 test3.c -lfirstlib -Wl,-rpath=`pwd`
	@echo 'SHARED LIBLARY\n' >> raport3b.txt
	./test3 raport3b.txt
	
	make clean
	@echo 'OPTIMIZATION -O2\n' >> raport3b.txt
	$(CC2) -c -fPIC firstlib.c
	$(CC2) -shared -o libfirstlib.so firstlib.o
	$(CC2) -L. -o test3 test3.c -lfirstlib -Wl,-rpath=`pwd`
	@echo 'SHARED LIBLARY\n' >> raport3b.txt
	./test3 raport3b.txt
	
	make clean
	@echo 'OPTIMIZATION -Os\n' >> raport3b.txt
	$(CCs) -c -fPIC firstlib.c
	$(CCs) -shared -o libfirstlib.so firstlib.o
	$(CCs) -L. -o test3 test3.c -lfirstlib -Wl,-rpath=`pwd`
	@echo 'SHARED LIBLARY\n' >> raport3b.txt
	./test3 raport3b.txt
	
	make clean
	@echo 'NO OPTIMIZATION\N' >> raport3b.txt
	$(CC) -fPIC -shared -o libfirstlib.so firstlib.c
	$(CC) -o test3 dynamic_test.c -ldl
	LD_LIBRARY_PATH=/home/alice/Desktop/SysOpy/lab1/zad3 ./test3 raport3b.txt
	
	make clean
	@echo 'OPTIMIZATION -O0\N' >> raport3b.txt
	$(CC0) -fPIC -shared -o libfirstlib.so firstlib.c
	$(CC0) -o test3 dynamic_test.c -ldl
	LD_LIBRARY_PATH=/home/alice/Desktop/SysOpy/lab1/zad3 ./test3 raport3b.txt
	
	make clean
	@echo 'OPTIMIZATION -O2\N' >> raport3b.txt
	$(CC2) -fPIC -shared -o libfirstlib.so firstlib.c
	$(CC2) -o test3 dynamic_test.c -ldl
	LD_LIBRARY_PATH=/home/alice/Desktop/SysOpy/lab1/zad3 ./test3 raport3b.txt
	
	make clean
	@echo 'OPTIMIZATION -Os\N' >> raport3b.txt
	$(CCs) -fPIC -shared -o libfirstlib.so firstlib.c
	$(CCs) -o test3 dynamic_test.c -ldl
	LD_LIBRARY_PATH=/home/alice/Desktop/SysOpy/lab1/zad3 ./test3 raport3b.txt
	
	make clean
