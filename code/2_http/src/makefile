SHELL:=/bin/bash

CC = gcc

GDB_FLAG = -g
CFLAGS   = -Wall -pedantic -I

INCLUDE  = ../include
SRC   = .
BIN   = ../bin
UTILITY = net_utility

all : wc09 wc10 wc11 wc_cache wc_cache2 wp wp_dbl_connection ws ws_chunked ws_cache ws_content ws_reflect wp wp_dbl_connection wp_2_connections1 wp_2_connections2

.PHONY: clean, %_r

%_r: $(BIN)/%
	./$<

% : $(SRC)/%.c $(BIN)/$(UTILITY).o
	$(CC) $(GDB_FLAG) -o $(BIN)/$@ $^ $(CFLAGS)$(INCLUDE)

$(BIN)/$(UTILITY).o : $(SRC)/$(UTILITY).c
	$(CC) -c -o $@ $< $(CFLAGS)$(INCLUDE)

clean:
	cd $(BIN) ; ls -p  | grep -v / | grep -v ".html" | xargs rm

