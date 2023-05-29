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

roster_t* roster_new();
void roster_addPlayer(roster_t* roster, player_t* player);
void roster_updateAllPlayers_Helper(void* arg, const char* key, void* item);
void roster_updateAllPlayers(roster_t* roster, grid_t* fullMap);

void* roster_getPlayerFromAddr_Helper(void* arg, const char* key, void* item);
player_t* roster_getPlayerFromAddr(roster_t* roster, addr_t playerAddr);
void roster_getPlayerFromID_Helper(void* arg, const char* key, void* item);
player_t* roster_getPlayerFromID(roster_t* roster, char playerID);

#endif // __ROSTER_H
