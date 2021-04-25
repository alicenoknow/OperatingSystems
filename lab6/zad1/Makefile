
CC = gcc -Wall

compile:
	$(CC) client.c -o client
	$(CC) server.c -o server
	

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.out
	rm -f server
	rm -f client
	
server:
	./server

client:
	./client

