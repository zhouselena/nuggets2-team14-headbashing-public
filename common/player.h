/* 
 * player.h - header file for Nuggets 'player' module
 * 
 * A 'player' is a client who is actively playing the game.
 * It includes information about the player's location, wallet,
 * and other important info to know about each player.
 *
 * Selena Zhou, Kyla Widodo, 23S
 */

#ifndef __PLAYER_H
#define __PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../support/message.h"
#include "grid.h"
#include "game.h"

/**************** global types ****************/
typedef struct player player_t;

/**************** functions ****************/

player_t* player_new();
void player_delete(player_t* player);

void player_setAddress(player_t* player, addr_t address);
void player_setName(player_t* player, char* name);
void player_initializeGridAndLocation(player_t* player, grid_t* visibleGrid, grid_t* visibleGold, int locationX, int locationY);

void player_moveUpAndDown(player_t* player, int steps, char resetMapSpot);
void player_moveLeftAndRight(player_t* player, int steps, char resetMapSpot);
void player_foundGoldNuggets(player_t* player, int numGold);
void player_updateVisibility(player_t* player, grid_t* fullMap, grid_t* goldMap);
void player_serverMapUpdate(player_t* player, game_t* game);

addr_t player_getAddr(player_t* player);
char player_getID(player_t* player);
char* player_getName(player_t* player);
grid_t* player_getMap(player_t* player);
grid_t* player_getVisibleGold(player_t* player);
int player_getXLocation(player_t* player);
int player_getYLocation(player_t* player);
int player_getGold(player_t* player);

#endif // __PLAYER_H
