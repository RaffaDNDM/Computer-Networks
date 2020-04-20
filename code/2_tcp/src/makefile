SHELL:=/bin/bash

CC = gcc

CFLAGS   = -Wall -I
LINKER   = gcc -o


INCLUDE  = ../include
SRC   = .
BIN   = ../bin
UTILITY = net_utility

all : wc09 wc10 wc11 ws 

.PHONY: clean, %_r

%_r: $(BIN)/%
	./$<


% : $(SRC)/%.c $(BIN)/$(UTILITY).o
	$(CC) -o $(BIN)/$@ $^ -I$(INCLUDE)

$(BIN)/$(UTILITY).o : $(SRC)/$(UTILITY).c
	$(CC) -c -o $@ $< -I$(INCLUDE)

clean:
	rm $(BIN)/*

