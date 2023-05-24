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
#include "common/message.h"

/**************** file-local global variables ****************/

static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles

/**************** global types ****************/

typedef struct game {
    hashtable_t* players;
    int numbPlayers;
    addr_t* spectator;
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

    game->spectator = NULL;
    game->remainingGold = GoldTotal;
    game->numbPlayers = 0;
}

void game_setGold(game_t* game) {

    int numbPiles = rand() % (GoldMaxNumPiles-GoldMinNumPiles+1) + GoldMinNumPiles;  // will generate between 0 and difference, then add to min
    
    // Initialize the game by dropping
    // at least GoldMinNumPiles and at most GoldMaxNumPiles gold piles on random room spots;
    // each pile shall have a random number of nuggets.

}

void game_addSpectator(game_t* game, addr_t* newSpectator) {
    if (message_isAddr(newSpectator)) {
        if (game->spectator != NULL) {
            message_send(game->spectator, "QUIT You have been replaced by a new spectator.")
        }
        game->spectator = newSpectator;
    }
}

void game_addPlayer(game_t* game, addr_t* player, char* message) {
    if (game->numbPlayers == MaxPlayers) {
        message_send(player, "QUIT Game is full: no more players can join.")
        return;
    }
    /* QUIT Sorry - you must provide player's name.
     * OK playerID
     * GRID nrows ncols
     * GOLD n p r
     * DISPLAY\nstring
     */
}

void game_keyPress(game_t* game, addr_t* player, char* message) {

}

/*
    add player (MaxNameLength, MaxPlayers put in use)
    add spectator
    Get player given address
    player move left
    player move right
    player move up
    player move down
    player found gold

    In the game:
    PLAY adds a player to the Game hashtable, QUIT if game is full or no name provided, OK if successfully added
    to move a player, you need to
        -get the player from the hashtable
        -use cmds from player module
 */
