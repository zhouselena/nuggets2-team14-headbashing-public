/* 
 * roster.h - header file for Nuggets 'roster' module
 * 
 * A 'roster' holds informations about all the players in the game.
 * It is essentially a hashtable for players.
 *
 * Selena Zhou, Kyla Widodo, 23S
 */

#ifndef __ROSTER_H
#define __ROSTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "../support/message.h"

/**************** global types ****************/
typedef struct roster roster_t;
typedef struct findPlayerPack findPlayerPack_t;

/**************** functions ****************/

/**************** roster_new ****************/
/* Mallocs space for a new roster, and initializes player set.
 */
roster_t* roster_new();

/**************** roster_addPlayer ****************/
/* Given a player, adds to roster. Return true if successful
 */
bool roster_addPlayer(roster_t* roster, player_t* player);

/**************** roster_updateAllPlayers ****************/
/* Given a server map update, updates the visible portion of map for each individual player.
 */
void roster_updateAllPlayers(roster_t* roster, game_t* fullMap);

/**************** roster_updateAllPlayersGold ****************/
/* Given a gold update, sends new gold message to all players.
 */
void roster_updateAllPlayersGold(roster_t* roster, game_t* fullMap);

/**************** roster_createGameMessage ****************/
/* At the game end, create and return game over message with player purses
 * ordered by who last entered the game.
 */
char* roster_createGameMessage(roster_t* roster);

/**************** roster_delete ****************/
/* Frees all information and deletes roster.
 */
void roster_delete(roster_t* roster);

/* get player from info functions */

player_t* roster_getPlayerFromAddr(roster_t* roster, addr_t playerAddr);
player_t* roster_getPlayerFromID(roster_t* roster, char playerID);

#endif // __ROSTER_H
