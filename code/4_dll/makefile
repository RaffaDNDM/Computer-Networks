SHELL:=/bin/bash

CC = gcc

CFLAGS = -Wall -pedantic

all: np nt

np : ping.c
	$(CC) -o $@ $^ $(CFLAGS)
	sudo setcap cap_net_raw,cap_net_admin=eip ./$@	

nt: traceroute.c
	$(CC) -o $@ $^ $(CFLAGS)
	sudo setcap cap_net_raw,cap_net_admin=eip ./$@

.PHONY: clean

clean:
	rm np nt
