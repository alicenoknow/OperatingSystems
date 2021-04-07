
CC = gcc -Wall

compile:
	$(CC) -c libka.c
	ar rcs libka.a libka.o
	$(CC) catcher.c libka.a -o catcher
	$(CC) sender.c libka.a -o sender
	

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.out
	rm -f sender
	rm -f catcher
	
run:
	 @echo "kill test 200 signals"
	./catcher kill&
	./sender 200 `pidof catcher` kill
	
	@echo "sigrt test 200 signals"
	./catcher sigrt&
	./sender 200 `pidof catcher` sigrt
	
	@echo "sigqueue test 200 signals"
	./catcher sigqueue&
	./sender 20 `pidof catcher` sigqueue

