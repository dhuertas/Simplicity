CC = g++
CFLAGS = -g -Wall
SHELL = /bin/bash

BASE =

TARGET = tictoc

INCLUDE = \
	-I../../src \
	-I../../src/base \
	-I../../src/json

# Base sources
SOURCES = $(shell find ../../src -name '*.cc')

# Local sources
SOURCES += $(shell find ./src -name '*.cc')

default:
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCES) -o $(TARGET)

clean:
	@rm -f $(TARGET)
	@rm -f $(shell find ../../src -name '*.o')