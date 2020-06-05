SHELL:=/bin/bash

CC = gcc

CFLAGS = -Wall -pedantic

np : ping.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean: np
	rm np
