/* 
 * gold.h - header file for Nuggets 'gold' module
 * 
 * 'gold' holds info for gold nuggets in a map.
 *
 * Selena Zhou, Kyla Widodo, 23S
 */

#ifndef __GOLD_H
#define __GOLD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"

/**************** global types ****************/
typedef struct goldPile goldPile_t;
typedef struct gold gold_t;

/**************** functions ****************/

/**************** gold_new ****************/
/* Allocates memory for a new set of gold piles.
 * Caller provides: total number of piles
 * Return: new gold information set
 */
gold_t* gold_new(int totalPiles);

/**************** gold_addGoldPile ****************/
/* Adds new gold pile information into gold info set.
 * Caller provides: valid gold set, XY location, numb nuggets
 * Return: nothing
 */
void gold_addGoldPile(gold_t* gold, int row, int col, int nuggets);

/**************** gold_foundPile ****************/
/* Given location, returns how many nuggets is in that pile
 * Caller provides: valid gold set, XY location
 * Return: number of gold nuggets in the pile
 */
int gold_foundPile(gold_t* gold, int row, int col);

/**************** gold_delete ****************/
/* Frees memory taken by gold and deletes gold.
 */
void gold_delete(gold_t* gold);

#endif // __GOLD_H
