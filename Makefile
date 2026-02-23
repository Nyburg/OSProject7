CC=gcc
CFLAGS=-Wall -Wextra -Werror -pthread
TARGET=grayscaler
SRCS=grayscaler.c ppm.c
OBJS=$(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

grayscaler.o: grayscaler.c ppm.h
ppm.o: ppm.c ppm.h

clean:
	rm -f $(OBJS)

pristine: clean
	rm -f $(TARGET)