

CC = gcc
CFLAGS = -Wall -Werror 
all: 
	$(CC) essh.c -o essh $(CFLAGS)