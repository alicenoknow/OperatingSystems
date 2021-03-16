CC = gcc -Wall

static:
	make clean
	$(CC) -c firstlib.c
	ar crs libfirstlib.a firstlib.o
	$(CC) test2.c -o test2 -L. -lfirstlib

shared:
	make clean
	$(CC) -c -fPIC firstlib.c
	$(CC) -shared -o libfirstlib.so firstlib.o
	$(CC) -L. -o test2 test2.c -lfirstlib -Wl,-rpath=`pwd`
	

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *.out
	rm -f test2
	rm -f tmp*.txt
	
run:
	./test2
	make clean


