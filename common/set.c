/* 
 * set.c - CS50 'set' module
 *
 * see set.h for more information.
 *
 * Selena Zhou, 23S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"

/**************** local types ****************/
typedef struct setnode {
    char* key;                // pointer to key
    void* item;               // pointer to data
    struct setnode *next;
} setnode_t;

/**************** global types ****************/
typedef struct set {
  struct setnode *head;       // head of the list of items in set
} set_t;

/**************** local functions ****************/
/* not visible outside this file */
static setnode_t* new_setnode(char* key, void* item) {
  setnode_t* node = malloc(sizeof(setnode_t));
  if (node == NULL) {     // error allocating memory for node; return error
    return NULL;
  }
  node->key = key;
  node->item = item;
  return node;
}

/**************** global functions ****************/
/* that is, visible outside this file */
/* see set.h for comments about exported functions */

/**************** set_new ****************/
/* see set.h for description */
set_t* set_new(void) {
  set_t* set = malloc(sizeof(set_t));
  if (set == NULL) {                  // error allocating memory for node; return error
    return NULL;
  } else {
    set->head = NULL;                 // initializes start of set
    return set;
  }
}

/**************** set_insert ****************/
/* see set.h for description */
bool set_insert(set_t* set, const char* key, void* item) {

  if (set != NULL && item != NULL && key != NULL) {

    // make key copy
    char* keyCopy = malloc(strlen(key)+1);
    strcpy(keyCopy, key);
    
    // make sure key doesn't exist
    if (set_find(set, keyCopy) == NULL) {
      setnode_t* insert = new_setnode(keyCopy, item);
      insert->next = set->head;
      set->head = insert;
      return true;
    }
    else {
      free(keyCopy);
    }
  }
  return false;
  
}

/**************** set_find ****************/
/* see set.h for description */
void* set_find(set_t* set, const char* key) {

  if (set != NULL && key != NULL) {
    for (setnode_t* currentNode = set->head; currentNode != NULL; currentNode = currentNode->next) {
      if (strcmp(key, currentNode->key) == 0) {
        return currentNode->item;
      }
    }
  }

  return NULL;
}

/**************** set_print ****************/
/* see set.h for description */
void set_print(set_t* set, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item) ) {
  
  if (fp != NULL && set != NULL) {
    fputc('{', fp);
    for (setnode_t* currentNode = set->head; currentNode != NULL; currentNode = currentNode->next) {
      if (itemprint != NULL) { // print the node's item 
        (*itemprint)(fp, currentNode->key, currentNode->item); 
        if (currentNode->next != NULL) fputc(',', fp);   // writes comma as long as not the last item
      }
    }
    fputc('}', fp);
    fputs("\n", fp);
  } else {
    fputs("(null)\n", fp);
  }

}

/**************** set_iterate ****************/
/* see set.h for description */
void set_iterate(set_t* set, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) ) {
  if (set != NULL && itemfunc != NULL) {
    for (setnode_t* currentNode = set->head; currentNode != NULL; currentNode = currentNode->next) {
      (*itemfunc)(arg, currentNode->key, currentNode->item);
    }
  }
}

/**************** set_delete ****************/
/* see set.h for description */
void set_delete(set_t* set, void (*itemdelete)(void* item) ) {
  if (set != NULL) {
    for (setnode_t* currentNode = set->head; currentNode != NULL; ) {
      if (itemdelete != NULL) {
        (*itemdelete)(currentNode->item);
      }
      setnode_t* nextNode = currentNode->next;
      free(currentNode->key);
      free(currentNode);
      currentNode = nextNode;
    }
    free(set);
  }
}