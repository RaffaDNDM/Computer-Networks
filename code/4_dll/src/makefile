SHELL:=/bin/bash

CC = gcc

GDB_FLAG = -g
CFLAGS   = #-Wall -pedantic

INCLUDE  = ../include
SRC   = .
BIN   = ../bin
UTILITY = $(BIN)/utility.o $(BIN)/arp.o

all : ping traceroute time_exceeded unreachable_dest statistics record_route tcp rr_1

.PHONY: clean, %_r

%_r: $(BIN)/%
	./$<

time_exceeded: $(SRC)/time_exceeded.c 
	$(CC) $(GDB_FLAG) -o $(BIN)/$@ $^ $(CFLAGS)
	sudo setcap cap_net_raw,cap_net_admin=eip $(BIN)/$@

unreachable_dest: $(SRC)/unreachable_dest.c
	$(CC) $(GDB_FLAG) -o $(BIN)/$@ $^ $(CFLAGS)
	sudo setcap cap_net_raw,cap_net_admin=eip $(BIN)/$@

rr_1: $(SRC)/rr_1.c
	$(CC) $(GDB_FLAG) -o $(BIN)/$@ $^ $(CFLAGS)
	sudo setcap cap_net_raw,cap_net_admin=eip $(BIN)/$@


%: $(SRC)/%.c $(UTILITY)
	$(CC) $(GDB_FLAG) -o $(BIN)/$@ $^ $(CFLAGS) -I$(INCLUDE)
	sudo setcap cap_net_raw,cap_net_admin=eip $(BIN)/$@

$(BIN)/utility.o : $(SRC)/utility.c
	$(CC) -c -o $@ $< $(CFLAGS) -I$(INCLUDE)

$(BIN)/arp.o : $(SRC)/arp.c
	$(CC) -c -o $@ $< $(CFLAGS) -I$(INCLUDE)

clean:
	rm $(BIN)/*

