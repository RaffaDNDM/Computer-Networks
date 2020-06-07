SHELL:=/bin/bash

CC = gcc

CFLAGS = -Wall -pedantic

np : ping.c
	$(CC) -o $@ $^ $(CFLAGS)
	sudo setcap cap_net_raw,cap_net_admin=eip ./np	

.PHONY: clean

clean: np
	rm np
