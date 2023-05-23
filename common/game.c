/* 
 * game.c - Nuggets 'game' module
 * 
 * See game.h for more information.
 *
 * Selena Zhou, Kyla Widodo, 23S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/grid.h"
#include "common/player.h"
#include "common/hashtable.h"

/**************** file-local global variables ****************/

static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles

/**************** global types ****************/

typedef struct game {
    hashtable_t* players;
    grid_t* fullMap;
    // may need a gold map here
    int remainingGold;
} game_t;

/**************** functions ****************/

game_t* game_new(char* mapFileName) {

    game_t* game = malloc(sizeof(game_t));
    if (game == NULL) return NULL;

    game->players = hagshtable_new(MaxPlayers);
    if (game->players == NULL) return NULL;

    game->fullMap = grid_fromFile(mapFileName);
    if (game->fullMap == NULL) return NULL;

    game->remainingGold = GoldTotal;
}

void game_setGold(game_t* game) {

    // Initialize the game by dropping
    // at least GoldMinNumPiles and at most GoldMaxNumPiles gold piles on random room spots;
    // each pile shall have a random number of nuggets.

}