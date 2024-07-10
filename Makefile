CC = gcc
CFLAGS = -g -Wall
SOURCE = $(wildcard *.c)
TARGET = s-talk

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(TARGET)