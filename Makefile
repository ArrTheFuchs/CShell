CC = gcc
CFLAGS = -Wall

compile:
	$(CC) $(CFLAGS) -o shell shell.c

run: compile
	./shell

clean:
	rm -f shell