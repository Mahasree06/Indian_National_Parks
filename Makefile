CC = gcc

CFLAGS = -Wall -Wextra -Iinclude
SQLITE = -lsqlite3
PTHREAD = -lpthread
CURL = -lcurl
XML_CFLAGS = $(shell pkg-config --cflags libxml-2.0)
XML_LIBS = $(shell pkg-config --libs libxml-2.0)

all: db_query park_threads parse_parks

db_query: src/db_query.c
	$(CC) $(CFLAGS) src/db_query.c -o db_query $(SQLITE)

park_threads: src/main.c src/queue.c
	$(CC) $(CFLAGS) src/main.c src/queue.c -o park_threads $(PTHREAD)

parse_parks: src/parse_parks.c
	$(CC) $(CFLAGS) $(XML_CFLAGS) src/parse_parks.c -o parse_parks $(XML_LIBS) $(CURL)

clean:
	rm -f db_query park_threads parse_parks

.PHONY: all clean
