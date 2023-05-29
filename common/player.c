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

/**************** file-local global variables ****************/

static char nextPlayer = 'A';
static int playerNum = 0;

/**************** global types ****************/

typedef struct player {
    addr_t playerAddress;
    char playerID;                  // unique ID starting from A, B, C...
    char* playerName;               // player real name that client inputs
    int playerXLocation;            // player location x value
    int playerYLocation;            // player location y value
    int numGold;                    // player wallet
    addr_t playerAddress;           // player's address for networking purposes
// add vision stuff here as required
} player_t;

/**************** functions ****************/

/* create and delete */

player_t* player_new(char* name) {
    
    player_t* player = malloc(sizeof(player_t));

    if (player == NULL) {       // defensive OR memory fails to malloc
        return NULL;
    }

    // give player a unique ID
    player->playerID = nextPlayer;
    nextPlayer += 1;

    player->playerName = name;
    player->numGold = 0;
    player->playerXLocation = x;
    player->playerYLocation = y;
    player->playerAddress = address;

    return player;
}

void player_delete(player_t* player) {
    free(player);
    // shouldn't have to delete anything else right? nothing else was malloc'd
}

void player_moveLeftAndRight(player_t* player, int steps) {
    if(player != NULL) {
        player->playerXLocation += steps;
    }
}

void player_foundGoldNuggets(player_t* player, int addedGold){
    if(player != NULL && addedGold > 0){
        player->numGold += addedGold;
    }
}

/* getter functions */

addr_t player_getAddr(player_t* player) {
    return player->playerAddress;
}

char player_getID(player_t* player) {
    if(player != NULL){
        return player->playerID;
    } 
    else {
        return -1;
    }
}

char* player_getName(player_t* player) {
    if(player != NULL){
        return player->playerName;
    } 
    else {
        return -1;
    }
}

int player_getXLocation(player_t* player) {
    if(player != NULL){
        return player->playerXLocation;
    } 
    else {
        return -1;
    }    
}

int player_getYLocation(player_t* player) {
    if(player != NULL){
        return player->playerYLocation;
    } 
    else {
        return -1;
    }    
}

int player_getGold(player_t* player) {
    return player->numGold;
}
