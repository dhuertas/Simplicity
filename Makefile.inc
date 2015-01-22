CC = g++
CFLAGS = -g -Wall -O3
SHELL = /bin/bash

TARGET = particles

INCLUDE = \
  -I../../src \
  -I../../src/json \
  -I../../src/webserver

LIBS = \
  -pthread

# Base sources
SOURCES = $(shell find ../../src -name '*.cc')

OBJS = $(shell find ../../src -name '*.cc' | sed 's/.cc/.o/g')
