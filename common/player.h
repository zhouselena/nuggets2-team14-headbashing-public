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

/* create and delete */

/**************** player_new ****************/
/* Mallocs space for a new player, creates unique ID for player, initialize other info.
 * 
 * Caller provides: nothing
 * Returns: new player struct
 */
player_t* player_new();

/**************** player_delete ****************/
/* Frees space taken by player name, and player itself.
 * 
 * Caller provides: player
 * Returns: nothing
 */
void player_delete(player_t* player);

/* setters */

/**************** player_setAddress ****************/
/* Given address, adds that to player information.
 */
void player_setAddress(player_t* player, addr_t address);

/**************** player_setName ****************/
/* Given malloc'd name, adds to player information. (Must be free'd in delete)
 */
void player_setName(player_t* player, char* name);

/**************** player_initializeGridAndLocation ****************/
/* Called when player is first created. Updates player's starting location and grids.
 */
void player_initializeGridAndLocation(player_t* player, grid_t* visibleGrid, grid_t* visibleGold, int locationX, int locationY);

/* update functions */

/**************** player_moveUpAndDown ****************/
/* Changes playerYlocation. If steps is negative, player moves up.
 */
void player_moveUpAndDown(player_t* player, int steps, char resetMapSpot);
/**************** player_moveLeftAndRight ****************/
/* Changes playerXlocation. If steps is negative, player moves left.
 */
void player_moveLeftAndRight(player_t* player, int steps, char resetMapSpot);
/**************** player_foundGoldNuggets ****************/
/* Adds numGold to player's numGold.
 */
void player_foundGoldNuggets(player_t* player, int numGold);
/**************** player_updateVisibility ****************/
/* Called after player x and y are updated. Adds visible map to the player's map.
 */
void player_updateVisibility(player_t* player, grid_t* fullMap, grid_t* goldMap);
/**************** player_serverMapUpdate ****************/
/* If server map updates, player updates any part of its existing visible map/visible gold.
 */
void player_serverMapUpdate(player_t* player, game_t* game);

/* getter functions */

/**************** player_getAddr ****************/
/* Returns player address. */
addr_t player_getAddr(player_t* player);

/**************** player_getID ****************/
/* Returns player ID. */
char player_getID(player_t* player);

/**************** player_getName ****************/
/* Returns player name. */
char* player_getName(player_t* player);

/**************** player_getXLocation ****************/
/* Returns player column */
int player_getXLocation(player_t* player);

/**************** player_getYLocation ****************/
/* Returns player row */
int player_getYLocation(player_t* player);

/**************** player_getMap ****************/
/* Return grid of player's visible map. */
grid_t* player_getMap(player_t* player);

/**************** player_getVisibleGold ****************/
/* Returns grid of player's visible gold. */
grid_t* player_getVisibleGold(player_t* player);

/**************** player_getGold ****************/
/* Returns player purse */
int player_getGold(player_t* player);

#endif // __PLAYER_H
