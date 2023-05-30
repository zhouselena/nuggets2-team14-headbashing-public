/* 
 * gold.c - Nuggets 'gold' module
 * 
 * See gold.h for more information.
 *
 * Selena Zhou, Kyla Widodo, 23S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gold.h"
#include "set.h"

/**************** file local global types ****************/

typedef struct findGoldPile {
    int findRow;
    int findCol;
    goldPile_t* matchedPile;
} findGoldPile_t;

char goldID = 'A';

/**************** global types ****************/

typedef struct goldPile {
    int goldRow;
    int goldCol;
    int numNuggets;
    int collected;
} goldPile_t;

typedef struct gold {
    int numPiles;
    set_t* piles;
} gold_t;

/**************** functions ****************/

/**************** gold_new ****************/
/* see gold.h for description */
gold_t* gold_new(int totalPiles) {

    gold_t* gold = malloc(sizeof(gold_t));
    if (gold == NULL) return NULL;

    gold->numPiles = totalPiles;
    gold->piles = set_new();

    return gold;

}

/**************** gold_addGoldPile ****************/
/* see gold.h for description */
void gold_addGoldPile(gold_t* gold, int row, int col, int nuggets) {

    goldPile_t* pile = malloc(sizeof(goldPile_t));
    if (pile == NULL) return;
    pile->goldRow = row;
    pile->goldCol = col;
    pile->numNuggets = nuggets;
    pile->collected = 0;
    set_insert(gold->piles, &goldID, pile);
    goldID++;

}

/**************** gold_foundPile_Helper ****************/
/* Opaque to users outside of this file.
 * Helper function to be passed into set_iterate.
 * If a gold pile's location matches the location trying to be found, then return gold pile.
 */
void gold_foundPile_Helper(void* arg, const char* key, void* item) {
    findGoldPile_t* goldInfoPack = arg;
    goldPile_t* currentPile = item;
    if (currentPile->goldRow == goldInfoPack->findRow && currentPile->goldCol == goldInfoPack->findCol) {
        goldInfoPack->matchedPile = currentPile;
    }
}

/**************** gold_foundPile ****************/
/* see gold.h for description */
int gold_foundPile(gold_t* gold, int row, int col) {

    findGoldPile_t* goldInfoPack = malloc(sizeof(findGoldPile_t));
    goldInfoPack->findRow = row; goldInfoPack->findCol = col; goldInfoPack->matchedPile = NULL;
    set_iterate(gold->piles, goldInfoPack, gold_foundPile_Helper);
    goldPile_t* foundPile = goldInfoPack->matchedPile;
    if (foundPile == NULL) return -1;
    foundPile->collected = 1;
    return foundPile->numNuggets;

}