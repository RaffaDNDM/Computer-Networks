SHELL:=/bin/bash

CC = gcc

CFLAGS = -Wall -pedantic

ping : ping.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean: ping
	rm ping
