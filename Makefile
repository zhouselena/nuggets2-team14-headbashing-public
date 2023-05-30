# Makefile for CS50 Nuggets
#
# Team 14 - Headbashing - Spring 2023

C = ./common
S = ./support
LIBS = -lncurses
LLIBS = $C/common.a $S/support.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

server: server.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -lm $(LIBS) -o $@

server.o: $C/grid.h $C/player.h $C/game.h $S/message.h

client: client.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -lm $(LIBS) -o $@ -lncurses

client.o: $C/grid.h $C/player.h $C/mem.h $S/message.h

############## valgrind ##########
valgrind:
#   valgrind --leak-check=full --show-leak-kinds=all ./server
	valgrind --leak-check=full --show-leak-kinds=all ./client 

.PHONY: all clean valgrind 

############## default: make all libs and programs ##########
all:
	make -C common
	make server
	make client

############### TAGS for emacs users ##########
TAGS:  Makefile */Makefile */*.c */*.h */*.md */*.sh
	etags $^

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	rm -f *.log
	rm -f *.nfs

#	make -C common clean
