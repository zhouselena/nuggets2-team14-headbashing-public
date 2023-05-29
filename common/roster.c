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

// Given a display message, sends message to all players in the roster
void roster_updateAllPlayers(char* message) {

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
    set_iterate(roster->players, playerPack, *roster_getPlayerFromID_Helper);
    return playerPack->foundPlayer;
}