
CC = gcc -Wall

compile:
	$(CC) main.c -o main -lpthread
	
all:
	make compile
	rm -f result.txt
	./main 1 blocks data/pepper.ascii.pgm data/pepper_output.ascii.pgm
	./main 2 blocks data/pepper.ascii.pgm data/pepper_output.ascii.pgm
	./main 4 blocks data/pepper.ascii.pgm data/pepper_output.ascii.pgm
	./main 8 blocks data/pepper.ascii.pgm data/pepper_output.ascii.pgm
	./main 16 blocks data/pepper.ascii.pgm data/pepper_output.ascii.pgm
	
	./main 1 numbers data/pepper.ascii.pgm data/pepper_output.ascii.pgm
	./main 2 numbers data/pepper.ascii.pgm data/pepper_output.ascii.pgm
	./main 4 numbers data/pepper.ascii.pgm data/pepper_output.ascii.pgm
	./main 8 numbers data/pepper.ascii.pgm data/pepper_output.ascii.pgm
	./main 16 numbers data/pepper.ascii.pgm data/pepper_output.ascii.pgm
	make clean

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.out
	rm -f main

	


