CC=gcc
FLAGS=-O0

all: benchmark.c time_util.h
	$(CC) $(FLAGS) -o bm benchmark.c
