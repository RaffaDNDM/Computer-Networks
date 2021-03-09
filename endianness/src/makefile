SHELL:=/bin/bash

CC = gcc
CPP = g++
JAVAC = javac

GDB_FLAG = -g

INCLUDE  = ../include
SRC   = .
BIN   = ../bin

.PHONY: clean

endianness: endianness.c endianness.cpp Endianness.java
	$(CC) $(GDB_FLAG) -o $(BIN)/endianness_c.o $(SRC)/endianness.c -I$(INCLUDE)
	$(CPP) $(GDB_FLAG) -o $(BIN)/endianness_cpp.o $(SRC)/endianness.cpp -I$(INCLUDE)
	$(JAVAC) -d $(BIN) Endianness.java

clean: $(BIN)/endianness_c.o $(BIN)/endianness_cpp.o $(BIN)/Endianness.class
	rm $(BIN)/endianness_c.o $(BIN)/endianness_cpp.o $(BIN)/Endianness.class
