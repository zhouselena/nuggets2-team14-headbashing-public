# Makefile for 'common'
# 
# This makefile will default build the common.a library.
# It will also have a clean target that removes files produced by Make.
#
# Team 14- Headbashing; Kyla Widodo, Selena Zhou, 23S

OBJS = player.o set.o grid.o roster.o mem.o gold.o game.o
LIB = common.a
S = ../support
LLIBS = $S/support.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

$(LIB): $(OBJS) $(LLIBS)
	ar cr $(LIB) -lm $(OBJS)

message.o: $S/message.h
grid.o: grid.h $S/message.h
game.o: $S/message.h grid.h player.h roster.h game.h gold.h
player.o: player.h
roster.o: roster.h $S/message.h player.h set.h game.h
gold.o: gold.h set.h
set.o: set.h
mem.o: mem.h

.PHONY: all clean

clean:
	rm -f core
	rm -f $(LIB) *~ *.o
