CC = gcc

CFLAGS = -Wall -Wextra -g

TARGET = Jenga

SRCS = main.c globals.c utils.c lexer.c parser.c vm.c gc.c

OBJS = $(SRCS:.c=.o)

HEADERS = jenga.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
