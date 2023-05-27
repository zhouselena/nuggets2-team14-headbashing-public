# Makefile for 'common'
# 
# This makefile will default build the common.a library.
# It will also have a clean target that removes files produced by Make.
#
# Selena Zhou, Kyla Widodo 23S

OBJS = game.o player.o hashtable.o set.o hash.o
# grid.o
LIB = common.a
S = ../support
LLIBS = $S/support.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

$(LIB): $(OBJS) $(LLIBS)
	ar cr $(LIB) $(OBJS)

message.o: $S/message.h
game.o: $S/message.h grid.h player.h hashtable.h game.h
# grid.o: grid.h $S/message.h
player.o: player.h
hashtable.o: hashtable.h set.h hash.h
hash.o: hash.h
set.o: set.h

.PHONY: all clean

clean:
	rm -f core
	rm -f $(LIB) *~ *.o
