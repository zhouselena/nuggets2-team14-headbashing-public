/* 
 * roster.c - Nuggets 'roster' module
 * 
 * See roster.h for more information.
 *
 * Selena Zhou, Kyla Widodo, 23S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "set.h"
#include "../support/message.h"
#include "game.h"

/**************** file-local global variables ****************/

/**************** global types ****************/

typedef struct roster {
    set_t* players;         // key: playerID    value: player_t*
} roster_t;

typedef struct findPlayerPack {
    addr_t matchAddress;
    char matchPlayerID;
    player_t* foundPlayer;
} findPlayerPack_t;

/**************** functions ****************/

roster_t* roster_new() {

    roster_t* roster = malloc(sizeof(roster_t));
    if (roster == NULL) return NULL;

    roster->players = set_new();
    return roster;

}

bool roster_addPlayer(roster_t* roster, player_t* player) {
    char playerID = player_getID(player);
    return set_insert(roster->players, &playerID, player);
}

// Given the full map, edits what already exists in the visible map
// then sends display command to player giving their specific visibility map and replacing their playerID with @
void roster_updateAllPlayers_Helper(void* arg, const char* key, void* item) {
    grid_t* fullMap = arg;
    player_t* currentPlayer = item;
    player_serverMapUpdate(currentPlayer, fullMap);

    grid_t* visibleGrid = player_getMap(currentPlayer);
    grid_t* visibleGold = player_getVisibleGold(currentPlayer);
    grid_overlay(visibleGrid, visibleGold, visibleGrid, visibleGrid);
    const char* gridString = grid_string(visibleGrid);
    char* sendDisplayMsg = malloc(strlen("DISPLAY") + strlen(gridString) + 5);
    sprintf(sendDisplayMsg, "DISPLAY\n%s", gridString);
    message_send(player_getAddr(currentPlayer), sendDisplayMsg);
    free(sendDisplayMsg);
}
void roster_updateAllPlayers(roster_t* roster, grid_t* fullMap) {
    set_iterate(roster->players, fullMap, roster_updateAllPlayers_Helper);
}

/* find player helpers */

void roster_getPlayerFromAddr_Helper(void* arg, const char* key, void* item) {
    findPlayerPack_t* playerPack = arg;
    player_t* currPlayer = item;
    if (message_eqAddr(playerPack->matchAddress, player_getAddr(currPlayer))) {
        playerPack->foundPlayer = currPlayer;
    }
}

player_t* roster_getPlayerFromAddr(roster_t* roster, addr_t playerAddr) {
    findPlayerPack_t* playerPack = malloc(sizeof(findPlayerPack_t));
    playerPack->matchAddress = playerAddr;
    playerPack->foundPlayer = NULL;
    set_iterate(roster->players, playerPack, *roster_getPlayerFromAddr_Helper);
    return playerPack->foundPlayer;
}

void roster_getPlayerFromID_Helper(void* arg, const char* key, void* item) {
    findPlayerPack_t* playerPack = arg;
    player_t* currPlayer = item;
    if (player_getID(currPlayer) == playerPack->matchPlayerID) {
        playerPack->foundPlayer = currPlayer;
    }
}

player_t* roster_getPlayerFromID(roster_t* roster, char playerID) {
    findPlayerPack_t* playerPack = malloc(sizeof(findPlayerPack_t));
    playerPack->matchPlayerID = playerID;
    playerPack->foundPlayer = NULL;
    set_iterate(roster->players, playerPack, *roster_getPlayerFromID_Helper);
    return playerPack->foundPlayer;
}