# Makefile for CS50 Nuggets
#
# Team 14 - Spring 2023

.PHONY: all clean

############## default: make all libs and programs ##########
all: 
	make -C common
	make -C client
	make -C server

############### TAGS for emacs users ##########
TAGS:  Makefile */Makefile */*.c */*.h */*.md */*.sh
	etags $^

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	make -C common clean
	make -C client clean
	make -C server clean