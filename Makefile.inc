CC = g++
CFLAGS = -g -Wall -O3
SHELL = /bin/bash

INCLUDE = \
  -I../../src \
  -I../../src/json \
  -I../../src/webserver

LIBS = 

UNAME_S = $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
  LIBS += -lpthread
endif

ifeq ($(UNAME_S),Linux)
  LIBS += -pthread
endif

# Base sources
SOURCES = $(shell find ../../src -name '*.cc')

OBJS = $(shell find ../../src -name '*.cc' | sed 's/.cc/.o/g')
