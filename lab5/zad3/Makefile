
CC = gcc -Wall

compile:
	$(CC) consumer.c -o consumer
	$(CC) producer.c -o producer
	$(CC) main.c -o main
	

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.out
	rm -f consumer
	rm -f producer
	rm -f main
	
run:
	./consumer ./fifo ./result.txt 10
	./producer ./fifo 2 ./data/r2.txt 10
	./producer ./fifo 3 ./data/r1.txt 10
	


