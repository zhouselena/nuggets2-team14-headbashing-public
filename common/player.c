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

/**************** global types ****************/
typedef struct player {
    char playerID;                  // unique ID starting from A, B, C...
    char* playerName;               // player real name that client inputs
    int playerXLocation;            // player location x value
    int playerYLocation;            // player location y value
    int numGold;                    // player wallet
    // add vision stuff here as required
} player_t;

/**************** functions ****************/

/* create and delete */

player_t* player_new(char* name) {
    
    player_t* player = malloc(sizeof(player_t));

    if (player == NULL || name == NULL) {       // defensive OR memory fails to malloc
        return NULL;
    }

    // give player a unique ID
    player->playerID = nextPlayer;
    nextPlayer += 1;

    player->playerName = name;
    player->numGold = 0;
    // TODO: playerXLocation
    // TODO: playerYLocation

    return player;

}

void player_delete(player_t* player) {
    free(player);
    // shouldn't have to delete anything else right? nothing else was malloc'd
}

/* update functions */

void player_moveUpAndDown(player_t* player, int steps);
void player_moveLeftAndRight(player_t* player, int steps);
void player_foundGoldNuggets(player_t* player, int numGold);

/* getter functions */

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

int player_getGold(player_t* player) {
    return player->numGold;
}
