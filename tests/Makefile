
src = $(wildcard *.c)
obj = $(src:.c=.o)
bin = 005010X221A1

CC = gcc
CFLAGS = -std=c17 -O2 -Wall -Wextra -I../lib
LDFLAGS = -L../lib -ledi

$(bin): $(obj)
	$(CC) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm $(bin) $(obj)
