/* 
 * client.h - header file for CS50 nuggests project client module
 *
 * The client is a program that allows a human to join the game as either player or spectator.
 *
 * Team 14 - headbashing, May 2023
 */

#ifndef __CLIENT_H
#define __CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include "mem.h"
#include <unistd.h>

/**************** hashtable_new ****************/
/* Create a new (empty) hashtable.
 *
 * Caller provides:
 *   number of slots to be used for the hashtable (must be > 0).
 * We return:
 *   pointer to the new hashtable; return NULL if error.
 * We guarantee:
 *   hashtable is initialized empty.
 * Caller is responsible for:
 *   later calling hashtable_delete.
 */
hashtable_t* hashtable_new(const int num_slots);

/**************** hashtable_insert ****************/
/* Insert item, identified by key (string), into the given hashtable.
 *
 * Caller provides:
 *   valid pointer to hashtable, valid string for key, valid pointer for item.
 * We return:
 *   false if key exists in ht, any parameter is NULL, or error;
 *   true iff new item was inserted.
 * Notes:
 *   The key string is copied for use by the hashtable; that is, the module
 *   is responsible for allocating memory for a copy of the key string, and
 *   later deallocating that memory; thus, the caller is free to re-use or 
 *   deallocate its key string after this call.  
 */
bool hashtable_insert(hashtable_t* ht, const char* key, void* item);
