SHELL:=/bin/bash

CC = gcc

GDB_FLAG = -g
CFLAGS   = -Wall -pedantic -I

INCLUDE  = ../include
SRC   = .
BIN   = ../bin
UTILITY = $(BIN)/utility.o $(BIN)/arp.o

all : np nt

.PHONY: clean, %_r

%_r: $(BIN)/%
	./$<

np : $(SRC)/ping.c $(UTILITY)
	$(CC) $(GDB_FLAG) -o $(BIN)/$@ $^ $(CFLAGS)$(INCLUDE)
	sudo setcap cap_net_raw,cap_net_admin=eip $(BIN)/$@

nt : $(SRC)/traceroute.c $(UTILITY)
	$(CC) $(GDB_FLAG) -o $(BIN)/$@ $^ $(CFLAGS)$(INCLUDE)
	sudo setcap cap_net_raw,cap_net_admin=eip $(BIN)/$@

$(BIN)/utility.o : $(SRC)/utility.c
	$(CC) -c -o $@ $< $(CFLAGS)$(INCLUDE)

$(BIN)/arp.o : $(SRC)/arp.c
	$(CC) -c -o $@ $< $(CFLAGS)$(INCLUDE)

clean:
	rm $(BIN)/*

