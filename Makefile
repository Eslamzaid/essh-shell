

CC = gcc
CFLAGS = -Wall -Werror 
LINK_WITH = path.c
all: 
	$(CC) essh.c $(LINK_WITH) -o wish $(CFLAGS)