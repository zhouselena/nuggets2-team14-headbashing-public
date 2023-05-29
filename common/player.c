/* 
 * player.c - Nuggets 'player' module
 * 
 * See player.h for more information.
 *
 * Selena Zhou, Kyla Widodo, 23S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../support/message.h"
#include "grid.h"

/**************** file-local global variables ****************/

static char nextPlayer = 'A';

/**************** global types ****************/

typedef struct player {
    addr_t playerAddress;
    char playerID;                  // unique ID starting from A, B, C...
    char* playerName;               // player real name that client inputs
    int playerXLocation;            // player location x value
    int playerYLocation;            // player location y value
    int numGold;                    // player wallet
    grid_t* visibleMap;             // player's visible map **REMEMBER THAT YOUR OWN LOCATION SHOULD BE @**
} player_t;

/**************** functions ****************/

/* create and delete */

player_t* player_new() {
    
    player_t* player = malloc(sizeof(player_t));

    if (player == NULL) {       // defensive OR memory fails to malloc
        return NULL;
    }

    // give player a unique ID
    player->playerID = nextPlayer;
    nextPlayer += 1;
    // start player purse with 0
    player->numGold = 0;

    return player;

}

void player_delete(player_t* player) {
    free(player->playerName);
    free(player);
}

/* set functions */

void player_setAddress(player_t* player, addr_t address) {
    player->playerAddress = address;
}

void player_setName(player_t* player, char* name) {
    player->playerName = name;
}

void player_initializeGridAndLocation(player_t* player, grid_t* visibleGrid, int locationX, int locationY) {
    player->visibleMap = visibleGrid;
    player->playerXLocation = locationX;
    player->playerYLocation = locationY;
}

/* update functions */

void player_moveUpAndDown(player_t* player, int steps, char resetMapSpot) {
    grid_set(player->visibleMap, player->playerYLocation, player->playerXLocation, resetMapSpot);
    player->playerYLocation += steps;
    grid_set(player->visibleMap, player->playerYLocation, player->playerXLocation, GRID_PLAYER_ME);
}
void player_moveLeftAndRight(player_t* player, int steps, char resetMapSpot) {
    grid_set(player->visibleMap, player->playerYLocation, player->playerXLocation, resetMapSpot);
    player->playerXLocation += steps;
    grid_set(player->visibleMap, player->playerYLocation, player->playerXLocation, GRID_PLAYER_ME);
}
void player_foundGoldNuggets(player_t* player, int numGold);

// called after player x and y are updated
void player_updateVisibility(player_t* player, grid_t* fullMap) {
    // grid_overlay(const grid_t* base, const grid_t* overlay, const grid_t* mask, grid_t* out)
    // base: player->visibleMap
    // overlay: grid_visible()
    // mask: fullMap
    // out: player->visibleMap
    grid_t* updatedVisible = grid_new(grid_nrows(fullMap), grid_ncols(fullMap));
    grid_visible(fullMap, player->playerYLocation, player->playerYLocation, updatedVisible);
    grid_overlay(player->visibleMap, updatedVisible, fullMap, player->visibleMap);
    grid_delete(updatedVisible);
}

void player_serverMapUpdate(player_t* player, grid_t* fullMap) {
    // grid_overlay(const grid_t* base, const grid_t* overlay, const grid_t* mask, grid_t* out)
    // base: player->visibleMap
    // overlay: fullMap
    // mask: player->visibleMap
    // out: player->visibleMap
    grid_overlay(player->visibleMap, fullMap, player->visibleMap, player->visibleMap);
    grid_set(player->visibleMap, player->playerYLocation, player->playerXLocation, GRID_PLAYER_ME);
}

/* getter functions */

addr_t player_getAddr(player_t* player) {
    return player->playerAddress;
}

char player_getID(player_t* player) {
    return player->playerID;
}

char* player_getName(player_t* player) {
    return player->playerName;
}

int player_getXLocation(player_t* player) {
    return player->playerXLocation;
}

int player_getYLocation(player_t* player) {
    return player->playerYLocation;
}

grid_t* player_getMap(player_t* player) {
    return player->visibleMap;
}

int player_getGold(player_t* player) {
    return player->numGold;
}

