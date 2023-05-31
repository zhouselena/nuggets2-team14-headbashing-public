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
#include "game.h"

/**************** file-local global variables ****************/

static char nextPlayer = 'A';            // to create unique ID

/**************** global types ****************/

typedef struct player {
    addr_t playerAddress;           // player address
    char playerID;                  // unique ID starting from A, B, C...
    char* playerName;               // player real name that client inputs
    int playerXLocation;            // player location x value
    int playerYLocation;            // player location y value
    int numGold;                    // player wallet
    grid_t* visibleMap;             // player's visible map
    grid_t* visibleGold;            // player's visible gold
} player_t;

/**************** functions ****************/

/* create and delete */

/**************** player_new ****************/
/* see player.h for description */
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

/**************** player_delete ****************/
/* see player.h for description */
void player_delete(player_t* player) {
    free(player->playerName);
    grid_delete(player->visibleMap);
    grid_delete(player->visibleGold);
    free(player);
}

/* setters */

/**************** player_setAddress ****************/
/* see player.h for description */
void player_setAddress(player_t* player, addr_t address) {
    player->playerAddress = address;
}

/**************** player_setName ****************/
/* see player.h for description */
void player_setName(player_t* player, char* name) {
    player->playerName = name;
}

/**************** player_initializeGridAndLocation ****************/
/* see player.h for description */
void player_initializeGridAndLocation(player_t* player, grid_t* visibleGrid, grid_t* goldMap, int locationX, int locationY) {
    player->visibleMap = visibleGrid;
    player->playerXLocation = locationX;
    player->playerYLocation = locationY;

    grid_t* visibleGold = grid_new(grid_nrows(goldMap), grid_ncols(goldMap));
    grid_overlay(visibleGold, goldMap, visibleGrid, visibleGold);
    player->visibleGold = visibleGold;
}

/* update functions */

/**************** player_moveUpAndDown ****************/
/* see player.h for description */
void player_moveUpAndDown(player_t* player, int steps, char resetMapSpot) {
    grid_set(player->visibleMap, player->playerYLocation, player->playerXLocation, resetMapSpot);
    player->playerYLocation += steps;
    grid_set(player->visibleMap, player->playerYLocation, player->playerXLocation, GRID_PLAYER_ME);
}
/**************** player_moveLeftAndRight ****************/
/* see player.h for description */
void player_moveLeftAndRight(player_t* player, int steps, char resetMapSpot) {
    grid_set(player->visibleMap, player->playerYLocation, player->playerXLocation, resetMapSpot);
    player->playerXLocation += steps;
    grid_set(player->visibleMap, player->playerYLocation, player->playerXLocation, GRID_PLAYER_ME);
}
/**************** player_foundGoldNuggets ****************/
/* see player.h for description */
void player_foundGoldNuggets(player_t* player, int foundGold) {
    player->numGold += foundGold;
}

/**************** player_updateVisibility ****************/
/* see player.h for description */
void player_updateVisibility(player_t* player, grid_t* fullMap, grid_t* goldMap) {
    // grid_overlay(const grid_t* base, const grid_t* overlay, const grid_t* mask, grid_t* out)
    // base: player->visibleMap
    // overlay: grid_visible()
    // mask: fullMap
    // out: player->visibleMap
    grid_t* updatedVisible = grid_new(grid_nrows(fullMap), grid_ncols(fullMap));
    grid_visible(fullMap, player->playerYLocation, player->playerXLocation, updatedVisible);
    grid_set(updatedVisible, player->playerYLocation, player->playerXLocation, GRID_PLAYER_ME);
    grid_overlay(player->visibleMap, updatedVisible, fullMap, player->visibleMap);

    grid_t* visibleGold = grid_new(grid_nrows(fullMap), grid_ncols(fullMap));
    grid_overlay(visibleGold, goldMap, updatedVisible, visibleGold);
    grid_delete(player->visibleGold);
    player->visibleGold = visibleGold;
    
    grid_delete(updatedVisible);
}

/* getter functions */

/**************** player_getAddr ****************/
/* see player.h for description */
addr_t player_getAddr(player_t* player) {
    return player->playerAddress;
}

/**************** player_getID ****************/
/* see player.h for description */
char player_getID(player_t* player) {
    return player->playerID;
}

/**************** player_getName ****************/
/* see player.h for description */
char* player_getName(player_t* player) {
    return player->playerName;
}

/**************** player_getXLocation ****************/
/* see player.h for description */
int player_getXLocation(player_t* player) {
    return player->playerXLocation;
}

/**************** player_getYLocation ****************/
/* see player.h for description */
int player_getYLocation(player_t* player) {
    return player->playerYLocation;
}

/**************** player_getMap ****************/
/* see player.h for description */
grid_t* player_getMap(player_t* player) {
    return player->visibleMap;
}

/**************** player_getVisibleGold ****************/
/* see player.h for description */
grid_t* player_getVisibleGold(player_t* player) {
    return player->visibleGold;
}

/**************** player_getGold ****************/
/* see player.h for description */
int player_getGold(player_t* player) {
    return player->numGold;
}

