CC = g++
CFLAGS = -g -Wall
SHELL = /bin/bash

TARGET = test

INCLUDE = \
	-I../src \
	-I../src/base \
	-I../src/json

SOURCES = $(shell find ../src -type f \( -name '*.cc' ! -name main.cc \) )

default:
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCES) ./test.cc -o $(TARGET)

clean:
	@rm -f $(TARGET)