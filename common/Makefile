# Makefile for 'common'
# 
# This makefile will default build the common.a library.
# It will also have a clean target that removes files produced by Make.
#
# Selena Zhou, Kyla Widodo 23S

# OBJS = game.o grid.o player.o hashtable.o set.o
OBJS = hashtable.o set.o
LIB = common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# game.o: game.h grid.h player.h hashtable.h ../support/message.h
# grid.o: grid.h ../support/message.h
# player.o: player.h
hashtable.o: hashtable.h set.h hash.h
set.o: set.h

.PHONY: all clean

clean:
	rm -f core
	rm -f $(LIB) *~ *.o
