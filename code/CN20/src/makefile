SHELL:=/bin/bash

CC = gcc
GDB_FLAG = -g
CFLAGS   = -Wall -pedantic

INCLUDE  = ../include
SRC   = .
BIN   = ../bin

all : wc09 wc10 wc11 wc_cache wp wp_dbl_connection ws ws_chunked ws_content ws_reflect wp wp_dbl_connection

.PHONY: clean

% : $(SRC)/%.c
	$(CC) $(GDB_FLAG) -o $(BIN)/$@ $^

clean:
	cd $(BIN) & ls -p $(BIN) | grep -v / | grep -v ".html" | xargs rm

