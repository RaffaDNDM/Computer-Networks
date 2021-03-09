SHELL:=/bin/bash

CC = gcc

GDB_FLAG = -g
CFLAGS   = #-Wall -pedantic

INCLUDE  = ../include
SRC   = .
BIN   = ../bin
UTILITY = $(BIN)/utility.o $(BIN)/arp.o

all : ping traceroute time_exceeded unreachable_dest statistics record_route tcp inverse_ping split_ping

.PHONY: clean, %_r

%_r: $(BIN)/%
	./$<

%: $(SRC)/%.c $(UTILITY)
	$(CC) $(GDB_FLAG) -o $(BIN)/$@ $^ $(CFLAGS) -I$(INCLUDE)
	sudo setcap cap_net_raw,cap_net_admin=eip $(BIN)/$@

$(BIN)/utility.o : $(SRC)/utility.c
	$(CC) -c -o $@ $< $(CFLAGS) -I$(INCLUDE)

$(BIN)/arp.o : $(SRC)/arp.c
	$(CC) -c -o $@ $< $(CFLAGS) -I$(INCLUDE)

clean:
	rm $(BIN)/*

