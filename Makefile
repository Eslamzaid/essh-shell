

CC = gcc
CFLAGS = -Wall -Werror 
LINK_WITH = buildInFunc.c
all: 
	$(CC) essh.c $(LINK_WITH) -o wish $(CFLAGS)
	clear
	./wish