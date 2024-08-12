

CC = gcc
CFLAGS = -Wall -Werror 
# LINK_WITH = path.c
# $(CC) essh.c $(LINK_WITH) -o wish $(CFLAGS)
all: 
	$(CC) essh.c -o wish $(CFLAGS)