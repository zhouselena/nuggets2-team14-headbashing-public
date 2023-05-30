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

gold_t* gold_new(int totalPiles);
void gold_addGoldPile(gold_t* gold, int row, int col, int nuggets);
int gold_foundPile(gold_t* gold, int row, int col);

#endif // __GOLD_H
