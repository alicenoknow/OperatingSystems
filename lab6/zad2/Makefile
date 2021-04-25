
CC = gcc -Wall

compile:
	$(CC) -c lib.c
	ar rcs lib.a lib.o
	$(CC) main.c lib.a -o main
	

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.out
	rm -f main
	
run:
	./main test1.txt
