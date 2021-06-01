
CC = gcc -Wall

compile:
	$(CC) -pthread client.c -o client
	$(CC)  -pthread server.c -o server
	

client:
	./client

server:
	./server

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.out
	rm -f server
	rm -f client
	


