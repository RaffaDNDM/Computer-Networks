SHELL:=/bin/bash

CC = gcc
GDB_FLAG = -g
CFLAGS   = -Wall -pedantic

INCLUDE  = ../include
SRC   = .
BIN   = ../bin

all : wc09 wc10 wc11 ws wp wp_dbl_connection

.PHONY: clean

% : $(SRC)/%.c
	$(CC) $(CFLAGS) $(GDB_FLAG) -o $(BIN)/$@ $^

clean:
	ls -p $(BIN) | grep -v / | grep -v ".html" | $(BIN)/xargs rm

