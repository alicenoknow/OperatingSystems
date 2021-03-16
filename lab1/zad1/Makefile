CC = gcc -Wall

static:
	make clean
	$(CC) -c firstlib.c
	ar crs libfirstlib.a firstlib.o
	$(CC) test.c -o test -L. -lfirstlib

shared:
	make clean
	$(CC) -c -fPIC firstlib.c
	$(CC) -shared -o libfirstlib.so firstlib.o
	$(CC) -L. -o test test.c -lfirstlib -Wl,-rpath=`pwd`
	

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *.out
	rm -f test
	rm -f tmp*.txt
	
run:
	@echo Test 0 - basic operations
	./test createArray 1 mergeFiles f1.txt:f2.txt createBlock 0 printBlocks removeRow 0 0 printBlocks

	make clean


