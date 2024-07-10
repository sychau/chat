CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

all: receiver conditions

receiver:
	gcc $(CFLAGS) receiver.c timings.c main.c -o receiver.o

conditions:
	gcc $(CFLAGS) signaller.c printer.c timings.c conditions.c -o conditions.o

run: build
	./main

valgrind: build
	valgrind --leak-check=full ./main.o

clean:
	rm *.o

