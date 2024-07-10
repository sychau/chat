CC = gcc
CFLAGS = -g -Wall
SOURCE = $(wildcard *.c)
TARGET = $(SOURCE:.c=.o)

all: $(TARGET)

# Compiling each .c file into its corresponding .o file.
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@ 

clean:
	rm *.o
