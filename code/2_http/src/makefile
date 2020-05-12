SHELL:=/bin/bash

CC = gcc

GDB_FLAG = -g
CFLAGS   = -Wall -pedantic -I

INCLUDE  = ../include
SRC   = .
BIN   = ../bin
UTILITY = net_utility

all : wc09 wc10 wc11 ws wp

.PHONY: clean, %_r

%_r: $(BIN)/%
	./$<


% : $(SRC)/%.c $(BIN)/$(UTILITY).o
	$(CC) $(GDB_FLAG) -o $(BIN)/$@ $^ $(CFLAGS)$(INCLUDE)

$(BIN)/$(UTILITY).o : $(SRC)/$(UTILITY).c
	$(CC) -c -o $@ $< $(CFLAGS)$(INCLUDE)

clean:
	rm $(BIN)/*

