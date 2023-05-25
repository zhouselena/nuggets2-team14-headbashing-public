# Makefile for CS50 Nuggets
#
# Team 14 - Spring 2023

C = ./common
S = ./support
LIBS =
LLIBS = $C/common.a $S/support.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

server: server.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

server.o: $C/grid.h $C/player.h $C/game.h $S/message.h

.PHONY: all clean

############## default: make all libs and programs ##########
all: 
	make -C common
	server
# 	make -C client
# 	make -C server

############### TAGS for emacs users ##########
TAGS:  Makefile */Makefile */*.c */*.h */*.md */*.sh
	etags $^

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	make -C common clean
#	make -C client clean
#	make -C server clean