
CC = gcc -Wall

compile:
	$(CC) -c firstlib.c
	ar crs libfirstlib.a firstlib.o
	$(CC) main.c -o main -L. -lfirstlib

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.out
	rm -f main
	rm -f tmp*.txt
	
run:
	./main

