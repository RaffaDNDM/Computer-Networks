SHELL:=/bin/bash

CC = gcc

CFLAGS = -Wall -pedantic

base64 : base64.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean: base64
	rm base64
