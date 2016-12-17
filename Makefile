CC=gcc
FLAGS=-O0

all: benchmark.c
	$(CC) $(FLAGS) -o bm benchmark.c
