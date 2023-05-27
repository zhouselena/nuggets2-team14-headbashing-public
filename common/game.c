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
// #include "grid.h"
#include "player.h"
#include "hashtable.h"
#include "../support/message.h"

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
    addr_t spectator;
    // grid_t* fullMap;
    // may need a gold map here
    int remainingGold;
} game_t;

typedef struct findPlayer {         // for find player
    addr_t matchAddress;
    player_t* foundPlayer;
} findPlayer_t;

/**************** functions ****************/

game_t* game_new(char* mapFileName) {

    game_t* game = malloc(sizeof(game_t));
    if (game == NULL) return NULL;

    game->players = hashtable_new(MaxPlayers);
    if (game->players == NULL) return NULL;

    // game->fullMap = grid_fromFile(mapFileName);
    // if (game->fullMap == NULL) return NULL;

    game->remainingGold = GoldTotal;
    game->numbPlayers = 0;

    return game;
}

// void game_setGold(game_t* game) {

//     int numbPiles = rand() % (GoldMaxNumPiles-GoldMinNumPiles+1) + GoldMinNumPiles;  // will generate between 0 and difference, then add to min
    
//     // Initialize the game by dropping
//     // at least GoldMinNumPiles and at most GoldMaxNumPiles gold piles on random room spots;
//     // each pile shall have a random number of nuggets.

// }

// /* find player */

// player_t* game_getPlayerFromAddr(game_t* game, addr_t addr) {

//     findPlayer_t* playerInfoPack = malloc(sizeof(findPlayer_t));
//     player_t* findThisPlayer;
//     playerInfoPack->matchAddress = addr;
//     playerInfoPack->foundPlayer = findThisPlayer;
//     hashtable_iterate(game->players, playerInfoPack, game_getPlayerFromAddr_Helper);
//     return findThisPlayer;

// }

// void* game_getPlayerFromAddr_Helper(void* arg, const char* key, void* item) {
//     findPlayer_t* playerInfoPack = arg;         // cast to info pack
//     player_t* currentPlayer = item;             // cast to player
//     if (message_eqAddr(player_getAddr(currentPlayer), playerInfoPack->matchAddress)) {
//         playerInfoPack->foundPlayer = currentPlayer;
//     }
// }

void game_addSpectator(game_t* game, addr_t newSpectator) {
    if (message_isAddr(newSpectator)) {
        if (message_isAddr(game->spectator)) {
            message_send(game->spectator, "QUIT You have been replaced by a new spectator.");
        }
        game->spectator = newSpectator;
    }
}

/* receive input */

void game_addPlayer(game_t* game, addr_t playerAddr, const char* message) {

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

    message_send(playerAddr, "OK");

    // // Create new player and send OK message
    // player_t* newPlayer = player_new();
    // player_setAddress(newPlayer, playerAddr);
    // char* setName = malloc(MaxNameLength);      // need to be free'd in player_delete
    // strncpy(setName, playerName, MaxNameLength);
    // player_setName(newPlayer, setName);

    // free(cmd);
    // free(playerName);

    // game->numbPlayers += 1;

    // char* sendOKmessage = malloc(strlen(10));
    // sprintf(sendOKmessage, "OK %c", player_getID(newPlayer));
    // message_send(playerAddr, sendOKmessage);

    // // Send information to client

    // /* 
    //  * GRID nrows ncols
    //  */

    // game_sendDisplayMessage(game, playerAddr);

}

// void game_sendDisplayMessage(game_t* game, addr_t player) {
//     /* 
//      * GOLD n p r
//      * DISPLAY\nstring
//      */
// }

/* key press helper functions */

void game_Q_quitGame(game_t* game, addr_t player, const char* message) {
    message_send(player, "Q key received.");
}
void game_h_moveLeft(game_t* game, addr_t player, const char* message) {
    message_send(player, "h key received.");
}
void game_l_moveRight(game_t* game, addr_t player, const char* message) {
    message_send(player, "l key received.");
}
void game_j_moveDown(game_t* game, addr_t player, const char* message) {
    message_send(player, "j key received.");
}
void game_k_moveUp(game_t* game, addr_t player, const char* message) {
    message_send(player, "k key received.");
}
void game_y_moveDiagUpLeft(game_t* game, addr_t player, const char* message) {
    message_send(player, "y key received.");
}
void game_u_moveDiagUpRight(game_t* game, addr_t player, const char* message) {
    message_send(player, "u key received.");
}
void game_b_moveDiagDownLeft(game_t* game, addr_t player, const char* message) {
    message_send(player, "b key received.");
}
void game_n_moveDiagDownRight(game_t* game, addr_t player, const char* message) {
    message_send(player, "n key received.");
}

void game_keyPress(game_t* game, addr_t player, const char* message) {

    switch(message[4]) {        // message is in format "KEY k"
        case 'Q':
            game_Q_quitGame(game, player, message);
            break;
        case 'h':
            game_h_moveLeft(game, player, message);
            break;
        case 'l':
            game_l_moveRight(game, player, message);
            break;
        case 'j':
            game_j_moveDown(game, player, message);
            break;
        case 'k':
            game_k_moveUp(game, player, message);
            break;
        case 'y':
            game_y_moveDiagUpLeft(game, player, message);
            break;
        case 'u':
            game_u_moveDiagUpRight(game, player, message);
            break;
        case 'b':
            game_b_moveDiagDownLeft(game, player, message);
            break;
        case 'n':
            game_n_moveDiagDownRight(game, player, message);
            break;
    }

}

/*
* char* command = ;
* switch (command) {
* case "PLAY":
*      QUIT Game is full: no more players can join.
*      QUIT Sorry - you must provide player's name.
*      OK playerID
*      GRID nrows ncols
*      GOLD n p r
*      DISPLAY\nstring
* case "SPECTATE":
* case "KEY"
*      DISPLAY
*      ERROR
*      QUIT Thanks for playing!
*      QUIT Thanks for watching!
* GRID nrows ncols
* GOLD n p r
* DISPLAY\nstring
* default:
*      ERROR explanation
* }
* 
* QUIT GAME OVER:
* A          4 Alice
* B         16 Bob
* C        230 Carol
*/