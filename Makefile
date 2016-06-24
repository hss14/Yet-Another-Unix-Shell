CFILES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJS = $(CFILES:.c=.o)
CC = gcc
CFLAGS = -lreadline -lncurses -Wall -g #-DHSSDEBUG

all: yaush 

yaush: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $^ $(CFLAGS) -c 
	
.PHONY: clean
clean: $(OBJS) yaush
	rm $^

.PHONY: test
test:
	./yaush
