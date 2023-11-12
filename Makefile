CC=gcc
OPT=-Wall -Wextra -std=c11 -O2  -Wpedantic
OBJS=

all: img-search

img-search: img-search.c $(OBJS)
	$(CC) $(OPT) $(OPT) img-search.c -o img-search $(OBJS)

%.o: %.c %.h
	$(CC) $(OPT) $(DBG_OPT) -c $< -o $@

clean:
	rm -f img-search $(OBJS)