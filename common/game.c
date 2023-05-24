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
    hashtable_t* players;       // holds char* playerID to player_t* player
    int numbPlayers;
    addr_t* spectator;
    grid_t* fullMap;
    // may need a gold map here
    int remainingGold;
} game_t;

typedef struct findPlayer {         // for find player
    addr_t* matchAddress;
    player_t* foundPlayer;
} findPlayer_t;

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
            message_send(game->spectator, "QUIT You have been replaced by a new spectator.");
        }
        game->spectator = newSpectator;
    }
}

void game_addPlayer(game_t* game, addr_t* playerAddr, char* message) {

    // Send QUIT message if at max players or no player name provided
    if (game->numbPlayers == MaxPlayers) {
        message_send(playerAddr, "QUIT Game is full: no more players can join.");
        return;
    }

    char* cmd = malloc(strlen(message));
    char* playerName = malloc(strlen(message));
    sscanf(message, "%s %s", cmd, playerName); // may have an issue for "" names

    if (strlen(playerName) == 0) {
        message_send(playerAddr, "QUIT Sorry - you must provide player's name.");
        return;
    }

    // Create new player and send OK message
    player_t* newPlayer = player_new();
    player_setAddress(newPlayer, playerAddr);
    char* setName = malloc(MaxNameLength);      // need to be free'd in player_delete
    strncpy(setName, playerName, MaxNameLength);
    player_setName(newPlayer, setName);

    free(cmd);
    free(playerName);

    game->numbPlayers += 1;

    char* sendOKmessage = malloc(strlen(10));
    sprintf(sendOKmessage, "OK %c", player_getID(newPlayer));
    message_send(playerAddr, sendOKmessage);

    // Send information to client

    /* 
     * GRID nrows ncols
     */

    game_sendDisplayMessage(game, playerAddr);

}

void game_sendDisplayMessage(game_t* game, addr_t* player) {
    /* 
     * GOLD n p r
     * DISPLAY\nstring
     */
}

void game_keyPress(game_t* game, addr_t* player, char* message) {

}

/* key press helper functions */

void game_Q_quitGame(game_t* game, addr_t* player, char* message);
void game_h_moveLeft(game_t* game, addr_t* player, char* message);
void game_l_moveRight(game_t* game, addr_t* player, char* message);
void game_j_moveDown(game_t* game, addr_t* player, char* message);
void game_k_moveUp(game_t* game, addr_t* player, char* message);
void game_y_moveDiagUpLeft(game_t* game, addr_t* player, char* message);
void game_u_moveDiagUpRight(game_t* game, addr_t* player, char* message);
void game_b_moveDiagDownLeft(game_t* game, addr_t* player, char* message);
void game_n_moveDiagDownRight(game_t* game, addr_t* player, char* message);

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

/* helpers */

player_t* game_getPlayerFromAddr(game_t* game, addr_t* addr) {

    findPlayer_t* playerInfoPack = malloc(sizeof(findPlayer_t));
    player_t* findThisPlayer;
    playerInfoPack->matchAddress = addr;
    playerInfoPack->foundPlayer = findThisPlayer;
    hashtable_iterate(game->players, playerInfoPack, game_getPlayerFromAddr_Helper);
    return findThisPlayer;

}

void* game_getPlayerFromAddr_Helper(void* arg, const char* key, void* item) {
    findPlayer_t* playerInfoPack = arg;         // cast to info pack
    player_t* currentPlayer = item;             // cast to player
    if (message_eqAddr(player_getAddr(currentPlayer), playerInfoPack->matchAddress)) {
        playerInfoPack->foundPlayer = currentPlayer;
    }
}