/* 
 * hashtable.c - CS50 'set' module
 * 
 * A *hashtable* is a set of (key,item) pairs.  It acts just like a set, 
 * but is far more efficient for large collections.
 * see hashtable.h for more information.
 *
 * Selena Zhou, 23S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "set.h"
#include "hash.h"

/**************** global type ****************/
typedef struct hashtable {
    int num_slots;
    set_t** table;
} hashtable_t;

/**************** functions ****************/

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
hashtable_t* hashtable_new(const int num_slots) {

    if (num_slots <= 0) {       // must have at least one slot
        return NULL;
    }

    // allocate memory and set hashtable size
    hashtable_t* hashtable = malloc(sizeof(hashtable_t));
    hashtable->num_slots = num_slots;
    hashtable->table = calloc(num_slots, sizeof(set_t*));

    if (hashtable == NULL || hashtable->table == NULL) {    // memory is properly allocated
        return NULL;
    }

    for (int i = 0; i < num_slots; i++) {
        hashtable->table[i] = set_new();
    }

    return hashtable;
}

/**************** hashtable_insert ****************/
/* Insert item, identified by key (string), into the given hashtable.
 *
 * Caller provides:
 *   valid pointer to hashtable, valid string for key, valid pointer for item.
 * We return:
 *   false if key exists in ht, any parameter is NULL, or error;
 *   true if new item was inserted.
 * Notes:
 *   The key string is copied for use by the hashtable; that is, the module
 *   is responsible for allocating memory for a copy of the key string, and
 *   later deallocating that memory; thus, the caller is free to re-use or 
 *   deallocate its key string after this call.  
 */
bool hashtable_insert(hashtable_t* ht, const char* key, void* item) {
    if (ht != NULL && key != NULL && item != NULL) {
        int slot = hash_jenkins(key, ht->num_slots);    // returns slot to hash in the item
        return set_insert(ht->table[slot], key, item);  // returns whether or not set_insert was successful
    }
    return false;
}

/**************** hashtable_find ****************/
/* Return the item associated with the given key.
 *
 * Caller provides:
 *   valid pointer to hashtable, valid string for key.
 * We return:
 *   pointer to the item corresponding to the given key, if found;
 *   NULL if hashtable is NULL, key is NULL, or key is not found.
 * Notes:
 *   the hashtable is unchanged by this operation.
 */
void* hashtable_find(hashtable_t* ht, const char* key) {
    if (ht != NULL && key != NULL) {
        int slot = hash_jenkins(key, ht->num_slots); // find the slot
        return set_find(ht->table[slot], key);       // returns if set_find was successful
    }
    return NULL;
}

/**************** hashtable_print ****************/
/* Print the whole table; provide the output file and func to print each item.
 * 
 * Caller provides:
 *   valid pointer to hashtable, 
 *   FILE open for writing,
 *   itemprint that can print a single (key, item) pair.
 * We print:
 *   nothing, if NULL fp.
 *   "(null)" if NULL ht.
 *   one line per hash slot, with no items, if NULL itemprint.
 *   otherwise, one line per hash slot, listing (key,item) pairs in that slot.
 * Note:
 *   the hashtable and its contents are not changed by this function,
 */
void hashtable_print(hashtable_t* ht, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item)) {
    if (ht != NULL && fp != NULL) {
        for (int i = 0; i < ht->num_slots; i++) {
            fprintf(fp, "Slot %d: ", i);
            set_print(ht->table[i], fp, (*itemprint));
        }
    } else {
        fputs("(null\n)", fp);
    }
}

/**************** hashtable_iterate ****************/
/* Iterate over all items in the table; in undefined order.
 *
 * Caller provides:
 *   valid pointer to hashtable, 
 *   arbitrary void*arg pointer,
 *   itemfunc that can handle a single (key, item) pair.
 * We do:
 *   nothing, if ht==NULL or itemfunc==NULL.
 *   otherwise, call the itemfunc once for each item, with (arg, key, item).
 * Notes:
 *   the order in which hashtable items are handled is undefined.
 *   the hashtable and its contents are not changed by this function,
 *   but the itemfunc may change the contents of the item.
 */
void hashtable_iterate(hashtable_t* ht, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) ) {
    if (ht != NULL && arg != NULL && itemfunc != NULL) {
        for (int i = 0; i < ht->num_slots; i++) {           // iterates over each slot
            set_iterate(ht->table[i], arg, (*itemfunc));    // iterates over the set in the slot
        }
    }
}

/**************** hashtable_delete ****************/
/* Delete hashtable, calling a delete function on each item.
 *
 * Caller provides:
 *   valid hashtable pointer,
 *   valid pointer to function that handles one item (may be NULL).
 * We do:
 *   if hashtable==NULL, do nothing.
 *   otherwise, unless itemfunc==NULL, call the itemfunc on each item.
 *   free all the key strings, and the hashtable itself.
 * Notes:
 *   We free the strings that represent key for each item, because 
 *   this module allocated that memory in hashtable_insert.
 */
void hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item) ) {
    if (ht != NULL) {
        for (int i = 0; i < ht->num_slots; i++) {
            set_delete(ht->table[i], (*itemdelete));
        }
        free(ht->table);
        free(ht);
    }
}
