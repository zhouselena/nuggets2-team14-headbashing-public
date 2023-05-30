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
#include <ctype.h>
#include "grid.h"
#include "player.h"
#include "roster.h"
#include "../support/message.h"
#include "gold.h"

/**************** file-local global variables ****************/

static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles

/**************** global types ****************/

typedef struct game {
    roster_t* players;       // holds char* playerID to player_t* player
    int numbPlayers;
    addr_t spectator;
    grid_t* originalMap;
    grid_t* fullMap;
    grid_t* goldMap;
    gold_t* goldNuggets;
    int mapRows;
    int mapCols;
    int remainingGold;
} game_t;

/**************** helper functions ****************/


// Initialize the game by dropping
// at least GoldMinNumPiles and at most GoldMaxNumPiles gold piles on random room spots;
// each pile shall have a random number of nuggets.
void game_setGold(game_t* game) {
    
    game->goldMap = grid_new(game->mapRows, game->mapCols);
    int numbPiles = rand() % (GoldMaxNumPiles-GoldMinNumPiles+1) + GoldMinNumPiles;  // will generate between 0 and difference, then add to min
    int maxNuggetsInPile = GoldTotal - numbPiles + 1;       // max is total gold - total piles + 1, need to update max
    int allocatedNuggets = 0;
    game->goldNuggets = gold_new(numbPiles);

    for (int i = 0; i < numbPiles; i++) {
        // generate random location
        int goldRow = rand() % game->mapRows;
        int goldCol = rand() % game->mapCols;
        while(!grid_isRoomSpot(game->fullMap, goldRow, goldCol) || grid_isGold(game->goldMap, goldRow, goldCol)) {
            goldRow = rand() % game->mapRows;
            goldCol = rand() % game->mapCols;
        }

        int numbNuggets;
        if (i == numbPiles-1) {     // if at last pile, allocate all remaining gold
            numbNuggets = GoldTotal - allocatedNuggets;
            maxNuggetsInPile = 0;
        } else {
            // generate random nugget number, then update max nuggets
            numbNuggets = rand() % maxNuggetsInPile + 1;
            allocatedNuggets += numbNuggets;
            maxNuggetsInPile = (GoldTotal - allocatedNuggets) - (numbPiles - i) + 1;       // new max is remaining gold - remaining piles + 1
        }

        // remember pile in gold set
        gold_addGoldPile(game->goldNuggets, goldRow, goldCol, numbNuggets);

        // update grid
        grid_set(game->goldMap, goldRow, goldCol, GRID_GOLD);
        
    }

}

void game_sendOKMessage(player_t* newPlayer, addr_t playerAddr) {
    char* sendOKmessage = malloc(10);
    sprintf(sendOKmessage, "OK %c", player_getID(newPlayer));
    message_send(playerAddr, sendOKmessage);
    free(sendOKmessage);
}

void game_sendGridMessage(game_t* game, addr_t player) {
    char* sendGridMessage = malloc(10);
    sprintf(sendGridMessage, "GRID %d %d", game->mapRows, game->mapCols);
    message_send(player, sendGridMessage);
    free(sendGridMessage);
}

void game_sendGoldMessage(game_t* game, addr_t player, int n, int p) {
    char* sendGoldMsg = malloc(20);
    sprintf(sendGoldMsg, "GOLD %d %d %d", n, p, game->remainingGold);
    message_send(player, sendGoldMsg);
    free(sendGoldMsg);
}

void game_updateAllUsersGold(game_t* game) {
    if (message_isAddr(game->spectator)) {
        game_sendGoldMessage(game, game->spectator, 0, 0);
    }
    roster_updateAllPlayersGold(game->players, game);
}

bool game_foundGold(game_t* game, player_t* player, int goldRow, int goldCol) {
    int numbNuggets = gold_foundPile(game->goldNuggets, goldRow, goldCol);
    game->remainingGold -= numbNuggets;
    // if game over
    if (game->remainingGold == 0) {
        end_game(game);
        return true;
    }
    // otherwise update
    grid_set(game->goldMap, goldRow, goldCol, GRID_BLANK);
    player_foundGoldNuggets(player, numbNuggets);
    int purse = player_getGold(player);
    game_sendGoldMessage(game, player_getAddr(player), numbNuggets, purse);
    // update spectator
        if (message_isAddr(game->spectator)) {
        game_sendGoldMessage(game, game->spectator, 0, 0);
    }
    game_updateAllUsersGold(game);
    return false;
}

// If player is spectator, sends full map, otherwise sends player's visible map
void game_sendDisplayMessage(game_t* game, addr_t player) {
    if (message_eqAddr(game->spectator, player)) {
        grid_t* sendDisplayGrid = grid_new(game->mapRows, game->mapCols);
        grid_overlay(game->fullMap, game->goldMap, game->fullMap, sendDisplayGrid);
        const char* gridString = grid_string(sendDisplayGrid);
        char* sendDisplayMsg = malloc(strlen("DISPLAY") + strlen(gridString) + 5);
        sprintf(sendDisplayMsg, "DISPLAY\n%s", gridString);
        message_send(player, sendDisplayMsg);
        free(sendDisplayMsg);
        grid_delete(sendDisplayGrid);
        return;
    }
    player_t* playerToUpdate = roster_getPlayerFromAddr(game->players, player);
    grid_t* visibleGrid = player_getMap(playerToUpdate);
    grid_t* visibleGold = player_getVisibleGold(playerToUpdate);
    grid_overlay(visibleGrid, visibleGold, visibleGrid, visibleGrid);
    const char* gridString = grid_string(visibleGrid);
    char* sendDisplayMsg = malloc(strlen("DISPLAY") + strlen(gridString) + 5);
    sprintf(sendDisplayMsg, "DISPLAY\n%s", gridString);
    message_send(player, sendDisplayMsg);
    free(sendDisplayMsg);
}

// Call roster_updateAllPlayers to send latest DISPLAY, calls sendDisplay to spectator
void game_updateAllUsers(game_t* game) {
    if (message_isAddr(game->spectator)) {
        game_sendDisplayMessage(game, game->spectator);
    }
    roster_updateAllPlayers(game->players, game);
}

/**************** functions ****************/

game_t* game_new(char* mapFileName) {

    game_t* game = malloc(sizeof(game_t));
    if (game == NULL) return NULL;

    game->players = roster_new();
    if (game->players == NULL) return NULL;

    game->fullMap = grid_fromFile(mapFileName);
    if (game->fullMap == NULL) return NULL;
    game->originalMap = grid_fromFile(mapFileName);
    game->mapRows = grid_nrows(game->fullMap);
    game->mapCols = grid_ncols(game->fullMap);

    game->remainingGold = GoldTotal;
    game->numbPlayers = 0;

    game_setGold(game);

    return game;
}

void end_game(game_t* game) {
    // send all clients game over message
    // free everything in game
}

/* receive input */

void game_addSpectator(game_t* game, addr_t newSpectator) {
    if (message_isAddr(newSpectator)) {

        // Make sure spectator isn't a player already
        if (roster_getPlayerFromAddr(game->players, newSpectator) != NULL) {
            message_send(newSpectator, "ERROR You are already a player.");
            return;
        }

        if (message_isAddr(game->spectator)) {
            message_send(game->spectator, "QUIT You have been replaced by a new spectator.");
        }
        game->spectator = newSpectator;
        game_sendGridMessage(game, newSpectator);
        game_sendGoldMessage(game, newSpectator, 0, 0);
        game_sendDisplayMessage(game, newSpectator);
    }
}

void game_addPlayer(game_t* game, addr_t playerAddr, const char* message) {

    // Send QUIT if at max players
    if (game->numbPlayers == MaxPlayers) {
        message_send(playerAddr, "QUIT Game is full: no more players can join.");
        return;
    }

    // Send ERROR if Spectator sends
    if (message_eqAddr(game->spectator, playerAddr)) {
        message_send(playerAddr, "ERROR Invalid key for spectator.");
        return;
    }

    char* cmd = malloc(strlen(message));
    char* playerName = malloc(strlen(message));
    sscanf(message, "%s %s", cmd, playerName); // may have an issue for "" names

    // Send QUIT if no player name provided
    if (strlen(playerName) == 0) {
        message_send(playerAddr, "QUIT Sorry - you must provide player's name.");
        return;
    }

    game->numbPlayers += 1;

    // Create new player
    player_t* newPlayer = player_new();
    player_setAddress(newPlayer, playerAddr);
    char* setName = malloc(MaxNameLength);      // need to be free'd in player_delete
    strncpy(setName, playerName, MaxNameLength);
    player_setName(newPlayer, setName);
    roster_addPlayer(game->players, newPlayer);
    free(cmd);
    free(playerName);

    /* Initialize player location
     * Randomly go through full grid, if it's an empty space then
     *      Change spot in full grid to playerID
     *      Update player XY
     *      Update player visible grid
     * game_updateAllUsers
     */
    int playerX = rand() % game->mapCols;
    int playerY = rand() % game->mapRows;
    // make sure is in valid room spot and NOT gold nugget
    while(!grid_isRoomSpot(game->fullMap, playerY, playerX)) {
        playerX = rand() % game->mapCols;
        playerY = rand() % game->mapRows;
    }
    grid_set(game->fullMap, playerY, playerX, player_getID(newPlayer));
    grid_t* playerVisibleGrid = grid_new(game->mapRows, game->mapCols);
    grid_visible(game->fullMap, playerY, playerX, playerVisibleGrid);
    grid_set(playerVisibleGrid, playerY, playerX, GRID_PLAYER_ME);

    player_initializeGridAndLocation(newPlayer, playerVisibleGrid, game->goldMap, playerX, playerY);
    player_updateVisibility(newPlayer, game->fullMap, game->goldMap);

    // Send 'OK playerID'
    game_sendOKMessage(newPlayer, playerAddr);
    
    // Send information to client (GRID, GOLD, DISPLAY)
    game_sendGridMessage(game, playerAddr);
    game_sendGoldMessage(game, playerAddr, 0, 0);
    game_sendDisplayMessage(game, playerAddr);

    game_updateAllUsers(game);

}

/* key press helper functions */

bool game_Q_quitGame(game_t* game, addr_t player, const char* message) {

    if (message_eqAddr(game->spectator, player)) {
        message_send(player, "QUIT Thanks for watching!");
        game->spectator = message_noAddr();
        return false;
    }

    game->numbPlayers -= 1;
    
    player_t* freePlayer = roster_getPlayerFromAddr(game->players, player);
    grid_set(game->fullMap, player_getYLocation(freePlayer), player_getXLocation(freePlayer), grid_get(game->originalMap, player_getYLocation(freePlayer), player_getXLocation(freePlayer)));
    player_setAddress(freePlayer, message_noAddr());
    message_send(player, "QUIT Thanks for playing!");
    game_updateAllUsers(game);

    return false;
    
}

bool game_h_moveLeft(game_t* game, addr_t player, const char* message) {

    if (message_eqAddr(game->spectator, player)) {
        message_send(player, "ERROR Invalid key.");
        return false;
    }

    // set up player
    player_t* calledPlayer = roster_getPlayerFromAddr(game->players, player);

    // make sure not out of bound
    int playerRow = player_getYLocation(calledPlayer);
    int newPlayerCol = player_getXLocation(calledPlayer)-1;
    if (newPlayerCol < 0) return false;

    // check what the next space is
    char moveFrom = grid_get(game->originalMap, playerRow, newPlayerCol+1);
    char moveTo = grid_get(game->fullMap, playerRow, newPlayerCol);
    if (grid_isSpot(game->fullMap, playerRow, newPlayerCol)) {
        
        // if gold, send gold update to all clients
        if (grid_isGold(game->goldMap, playerRow, newPlayerCol)) {
            if (game_foundGold(game, calledPlayer, playerRow, newPlayerCol)) return true;
        }
        grid_set(game->fullMap, playerRow, newPlayerCol+1, moveFrom);                    // reset spot on map
        grid_set(game->fullMap, playerRow, newPlayerCol, player_getID(calledPlayer));    // update player on map
        player_moveLeftAndRight(calledPlayer, -1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        game_updateAllUsers(game);

    } else if (isalpha(moveTo)) {           // is another player then swap
        player_t* conflictingPlayer = roster_getPlayerFromID(game->players, moveTo);
        grid_set(game->fullMap, playerRow, newPlayerCol+1, moveTo);                    // reset spot on map
        grid_set(game->fullMap, playerRow, newPlayerCol, player_getID(calledPlayer));    // update player on map
        // update player
        player_moveLeftAndRight(calledPlayer, -1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        // update conflicting player
        player_moveLeftAndRight(conflictingPlayer, 1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_updateVisibility(conflictingPlayer, game->fullMap, game->goldMap);
        // update all
        game_updateAllUsers(game);
    }

    return false;

    // note: when player XY is changed, call player update visibility
    /* Check what the next location char is
     * If is wall or corner or empty, do nothing
     * else
     *      if is valid empty spot
     *          update player XY
     *          update player visible map
     *      if is other player, swap spots
     *          update player XY
     *          get other player from findplayerID, update other place XY
     *      if is gold
     *          update player XY
     *          update player gold coin
     *          update global coin count
     *          send gold update to all users
     *          
     *      ALL: update FullMap with player icon
     *           game_updateAllUsers 
     */
}
bool game_l_moveRight(game_t* game, addr_t player, const char* message) {

    if (message_eqAddr(game->spectator, player)) {
        message_send(player, "ERROR Invalid key for spectator.");
        return false;
    }

    // set up player
    player_t* calledPlayer = roster_getPlayerFromAddr(game->players, player);

    // make sure not out of bound
    int playerRow = player_getYLocation(calledPlayer);
    int newPlayerCol = player_getXLocation(calledPlayer)+1;
    if (newPlayerCol > game->mapCols) return false;

    // check what the next space is
    char moveFrom = grid_get(game->originalMap, playerRow, newPlayerCol-1);
    char moveTo = grid_get(game->fullMap, playerRow, newPlayerCol);
    if (grid_isSpot(game->fullMap, playerRow, newPlayerCol)) {
        
        // if gold, send gold update to all clients
        if (grid_isGold(game->goldMap, playerRow, newPlayerCol)) {
            if (game_foundGold(game, calledPlayer, playerRow, newPlayerCol)) return true;
        }
        grid_set(game->fullMap, playerRow, newPlayerCol-1, moveFrom);                    // reset spot on map
        grid_set(game->fullMap, playerRow, newPlayerCol, player_getID(calledPlayer));    // update player on map
        player_moveLeftAndRight(calledPlayer, 1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        game_updateAllUsers(game);

    } else if (isalpha(moveTo)) {           // is another player then swap
        player_t* conflictingPlayer = roster_getPlayerFromID(game->players, moveTo);
        grid_set(game->fullMap, playerRow, newPlayerCol-1, moveTo);                    // reset spot on map
        grid_set(game->fullMap, playerRow, newPlayerCol, player_getID(calledPlayer));    // update player on map
        // update player
        player_moveLeftAndRight(calledPlayer, 1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        // update conflicting player
        player_moveLeftAndRight(conflictingPlayer, -1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_updateVisibility(conflictingPlayer, game->fullMap, game->goldMap);
        // update all
        game_updateAllUsers(game);
    }

    return false;
}
bool game_j_moveDown(game_t* game, addr_t player, const char* message) {

    if (message_eqAddr(game->spectator, player)) {
        message_send(player, "ERROR Invalid key for spectator.");
        return false;
    }

    // set up player
    player_t* calledPlayer = roster_getPlayerFromAddr(game->players, player);

    // make sure not out of bound
    int newPlayerRow = player_getYLocation(calledPlayer)+1;
    int playerCol = player_getXLocation(calledPlayer);
    if (newPlayerRow > game->mapRows) return false;

    // check what the next space is
    char moveFrom = grid_get(game->originalMap, newPlayerRow-1, playerCol);
    char moveTo = grid_get(game->fullMap, newPlayerRow, playerCol);
    if (grid_isSpot(game->fullMap, newPlayerRow, playerCol)) {
        
        // if gold, send gold update to all clients
        if (grid_isGold(game->goldMap, newPlayerRow, playerCol)) {
            if (game_foundGold(game, calledPlayer, newPlayerRow, playerCol)) return true;
        }
        grid_set(game->fullMap, newPlayerRow-1, playerCol, moveFrom);                    // reset spot on map
        grid_set(game->fullMap, newPlayerRow, playerCol, player_getID(calledPlayer));    // update player on map
        player_moveUpAndDown(calledPlayer, 1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        game_updateAllUsers(game);

    } else if (isalpha(moveTo)) {           // is another player then swap
        player_t* conflictingPlayer = roster_getPlayerFromID(game->players, moveTo);
        grid_set(game->fullMap, newPlayerRow-1, playerCol, moveTo);                    // reset spot on map
        grid_set(game->fullMap, newPlayerRow, playerCol, player_getID(calledPlayer));    // update player on map
        // update player
        player_moveUpAndDown(calledPlayer, 1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        // update conflicting player
        player_moveUpAndDown(conflictingPlayer, -1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_updateVisibility(conflictingPlayer, game->fullMap, game->goldMap);
        // update all
        game_updateAllUsers(game);
    }

    return false;
    
}
bool game_k_moveUp(game_t* game, addr_t player, const char* message) {

    if (message_eqAddr(game->spectator, player)) {
        message_send(player, "ERROR Invalid key for spectator.");
        return false;
    }

    // set up player
    player_t* calledPlayer = roster_getPlayerFromAddr(game->players, player);

    // make sure not out of bound
    int newPlayerRow = player_getYLocation(calledPlayer)-1;
    int playerCol = player_getXLocation(calledPlayer);
    if (newPlayerRow < 0) return false;

    // check what the next space is
    char moveFrom = grid_get(game->originalMap, newPlayerRow+1, playerCol);
    char moveTo = grid_get(game->fullMap, newPlayerRow, playerCol);
    if (grid_isSpot(game->fullMap, newPlayerRow, playerCol)) {
        
        // if gold, send gold update to all clients
        if (grid_isGold(game->goldMap, newPlayerRow, playerCol)) {
            if (game_foundGold(game, calledPlayer, newPlayerRow, playerCol)) return true;
        }
        grid_set(game->fullMap, newPlayerRow+1, playerCol, moveFrom);                    // reset spot on map
        grid_set(game->fullMap, newPlayerRow, playerCol, player_getID(calledPlayer));    // update player on map
        player_moveUpAndDown(calledPlayer, -1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        game_updateAllUsers(game);

    } else if (isalpha(moveTo)) {           // is another player then swap
        player_t* conflictingPlayer = roster_getPlayerFromID(game->players, moveTo);
        grid_set(game->fullMap, newPlayerRow+1, playerCol, moveTo);                    // reset spot on map
        grid_set(game->fullMap, newPlayerRow, playerCol, player_getID(calledPlayer));    // update player on map
        // update player
        player_moveUpAndDown(calledPlayer, -1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        // update conflicting player
        player_moveUpAndDown(conflictingPlayer, 1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_updateVisibility(conflictingPlayer, game->fullMap, game->goldMap);
        // update all
        game_updateAllUsers(game);
    }

    return false;

}
bool game_y_moveDiagUpLeft(game_t* game, addr_t player, const char* message) {

    if (message_eqAddr(game->spectator, player)) {
        message_send(player, "ERROR Invalid key for spectator.");
        return false;
    }
    
    // set up player
    player_t* calledPlayer = roster_getPlayerFromAddr(game->players, player);

    // make sure not out of bound
    int newPlayerRow = player_getYLocation(calledPlayer)-1;
    int newPlayerCol = player_getXLocation(calledPlayer)-1;
    if (newPlayerRow < 0 || newPlayerCol < 0) return false;

    // check what the next space is
    char moveFrom = grid_get(game->originalMap, newPlayerRow+1, newPlayerCol+1);
    char moveTo = grid_get(game->fullMap, newPlayerRow, newPlayerCol);
    if (grid_isSpot(game->fullMap, newPlayerRow, newPlayerCol)) {
        
        // if gold, send gold update to all clients
        if (grid_isGold(game->goldMap, newPlayerRow, newPlayerCol)) {
            if (game_foundGold(game, calledPlayer, newPlayerRow, newPlayerCol)) return true;
        }
        grid_set(game->fullMap, newPlayerRow+1, newPlayerCol+1, moveFrom);                      // reset spot on map
        grid_set(game->fullMap, newPlayerRow, newPlayerCol, player_getID(calledPlayer));        // update player on map
        player_moveUpAndDown(calledPlayer, -1, moveFrom);
        moveFrom = grid_get(game->originalMap, player_getYLocation(calledPlayer), player_getXLocation(calledPlayer));
        player_moveLeftAndRight(calledPlayer, -1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        game_updateAllUsers(game);

    } else if (isalpha(moveTo)) {           // is another player then swap
        player_t* conflictingPlayer = roster_getPlayerFromID(game->players, moveTo);
        grid_set(game->fullMap, newPlayerRow+1, newPlayerCol+1, moveTo);                      // reset spot on map
        grid_set(game->fullMap, newPlayerRow, newPlayerCol, player_getID(calledPlayer));        // update player on map
        // update player
        player_moveUpAndDown(calledPlayer, -1, moveFrom);
        moveFrom = grid_get(game->originalMap, player_getYLocation(calledPlayer), player_getXLocation(calledPlayer));
        player_moveLeftAndRight(calledPlayer, -1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        // update conflicting player
        player_moveUpAndDown(conflictingPlayer, 1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_moveLeftAndRight(conflictingPlayer, 1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_updateVisibility(conflictingPlayer, game->fullMap, game->goldMap);
        // update all
        game_updateAllUsers(game);
    }

    return false;

}
bool game_u_moveDiagUpRight(game_t* game, addr_t player, const char* message) {

    if (message_eqAddr(game->spectator, player)) {
        message_send(player, "ERROR Invalid key for spectator.");
        return false;
    }

    // set up player
    player_t* calledPlayer = roster_getPlayerFromAddr(game->players, player);

    // make sure not out of bound
    int newPlayerRow = player_getYLocation(calledPlayer)-1;
    int newPlayerCol = player_getXLocation(calledPlayer)+1;
    if (newPlayerRow < 0 || newPlayerCol > game->mapCols) return false;

    // check what the next space is
    char moveFrom = grid_get(game->originalMap, newPlayerRow+1, newPlayerCol-1);
    char moveTo = grid_get(game->fullMap, newPlayerRow, newPlayerCol);
    if (grid_isSpot(game->fullMap, newPlayerRow, newPlayerCol)) {
        
        // if gold, send gold update to all clients
        if (grid_isGold(game->goldMap, newPlayerRow, newPlayerCol)) {
            if (game_foundGold(game, calledPlayer, newPlayerRow, newPlayerCol)) return true;
        }
        grid_set(game->fullMap, newPlayerRow+1, newPlayerCol-1, moveFrom);                      // reset spot on map
        grid_set(game->fullMap, newPlayerRow, newPlayerCol, player_getID(calledPlayer));        // update player on map
        player_moveUpAndDown(calledPlayer, -1, moveFrom);
        moveFrom = grid_get(game->originalMap, player_getYLocation(calledPlayer), player_getXLocation(calledPlayer));
        player_moveLeftAndRight(calledPlayer, 1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        game_updateAllUsers(game);

    } else if (isalpha(moveTo)) {           // is another player then swap
        player_t* conflictingPlayer = roster_getPlayerFromID(game->players, moveTo);
        grid_set(game->fullMap, newPlayerRow+1, newPlayerCol-1, moveTo);                      // reset spot on map
        grid_set(game->fullMap, newPlayerRow, newPlayerCol, player_getID(calledPlayer));        // update player on map
        // update player
        player_moveUpAndDown(calledPlayer, -1, moveFrom);
        moveFrom = grid_get(game->originalMap, player_getYLocation(calledPlayer), player_getXLocation(calledPlayer));
        player_moveLeftAndRight(calledPlayer, 1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        // update conflicting player
        player_moveUpAndDown(conflictingPlayer, 1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_moveLeftAndRight(conflictingPlayer, -1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_updateVisibility(conflictingPlayer, game->fullMap, game->goldMap);
        // update all
        game_updateAllUsers(game);
    }

    return false;
}
bool game_b_moveDiagDownLeft(game_t* game, addr_t player, const char* message) {

    if (message_eqAddr(game->spectator, player)) {
        message_send(player, "ERROR Invalid key for spectator.");
        return false;
    }

    // set up player
    player_t* calledPlayer = roster_getPlayerFromAddr(game->players, player);

    // make sure not out of bound
    int newPlayerRow = player_getYLocation(calledPlayer)+1;
    int newPlayerCol = player_getXLocation(calledPlayer)-1;
    if (newPlayerRow > game->mapRows || newPlayerCol < 0) return false;

    // check what the next space is
    char moveFrom = grid_get(game->originalMap, newPlayerRow-1, newPlayerCol+1);
    char moveTo = grid_get(game->fullMap, newPlayerRow, newPlayerCol);
    if (grid_isSpot(game->fullMap, newPlayerRow, newPlayerCol)) {
        
        // if gold, send gold update to all clients
        if (grid_isGold(game->goldMap, newPlayerRow, newPlayerCol)) {
            if (game_foundGold(game, calledPlayer, newPlayerRow, newPlayerCol)) return true;
        }
        grid_set(game->fullMap, newPlayerRow-1, newPlayerCol+1, moveFrom);                      // reset spot on map
        grid_set(game->fullMap, newPlayerRow, newPlayerCol, player_getID(calledPlayer));        // update player on map
        player_moveUpAndDown(calledPlayer, 1, moveFrom);
        moveFrom = grid_get(game->originalMap, player_getYLocation(calledPlayer), player_getXLocation(calledPlayer));
        player_moveLeftAndRight(calledPlayer, -1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        game_updateAllUsers(game);

    } else if (isalpha(moveTo)) {           // is another player then swap
        player_t* conflictingPlayer = roster_getPlayerFromID(game->players, moveTo);
        grid_set(game->fullMap, newPlayerRow-1, newPlayerCol+1, moveTo);                      // reset spot on map
        grid_set(game->fullMap, newPlayerRow, newPlayerCol, player_getID(calledPlayer));        // update player on map
        // update player
        player_moveUpAndDown(calledPlayer, 1, moveFrom);
        moveFrom = grid_get(game->originalMap, player_getYLocation(calledPlayer), player_getXLocation(calledPlayer));
        player_moveLeftAndRight(calledPlayer, -1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        // update conflicting player
        player_moveUpAndDown(conflictingPlayer, -1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_moveLeftAndRight(conflictingPlayer, 1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_updateVisibility(conflictingPlayer, game->fullMap, game->goldMap);
        // update all
        game_updateAllUsers(game);
    }

    return false;
}
bool game_n_moveDiagDownRight(game_t* game, addr_t player, const char* message) {

    if (message_eqAddr(game->spectator, player)) {
        message_send(player, "ERROR Invalid key for spectator.");
        return false;
    }

    // set up player
    player_t* calledPlayer = roster_getPlayerFromAddr(game->players, player);

    // make sure not out of bound
    int newPlayerRow = player_getYLocation(calledPlayer)+1;
    int newPlayerCol = player_getXLocation(calledPlayer)+1;
    if (newPlayerRow > game->mapRows || newPlayerCol > game->mapCols) return false;

    // check what the next space is
    char moveFrom = grid_get(game->originalMap, newPlayerRow-1, newPlayerCol-1);
    char moveTo = grid_get(game->fullMap, newPlayerRow, newPlayerCol);
    if (grid_isSpot(game->fullMap, newPlayerRow, newPlayerCol)) {
        
        // if gold, send gold update to all clients
        if (grid_isGold(game->goldMap, newPlayerRow, newPlayerCol)) {
            if (game_foundGold(game, calledPlayer, newPlayerRow, newPlayerCol)) return true;
        }
        grid_set(game->fullMap, newPlayerRow-1, newPlayerCol-1, moveFrom);                      // reset spot on map
        grid_set(game->fullMap, newPlayerRow, newPlayerCol, player_getID(calledPlayer));        // update player on map
        player_moveUpAndDown(calledPlayer, 1, moveFrom);
        moveFrom = grid_get(game->originalMap, player_getYLocation(calledPlayer), player_getXLocation(calledPlayer));
        player_moveLeftAndRight(calledPlayer, 1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        game_updateAllUsers(game);

    } else if (isalpha(moveTo)) {           // is another player then swap
        player_t* conflictingPlayer = roster_getPlayerFromID(game->players, moveTo);
        grid_set(game->fullMap, newPlayerRow-1, newPlayerCol-1, moveTo);                      // reset spot on map
        grid_set(game->fullMap, newPlayerRow, newPlayerCol, player_getID(calledPlayer));        // update player on map
        // update player
        player_moveUpAndDown(calledPlayer, 1, moveFrom);
        moveFrom = grid_get(game->originalMap, player_getYLocation(calledPlayer), player_getXLocation(calledPlayer));
        player_moveLeftAndRight(calledPlayer, 1, moveFrom);
        player_updateVisibility(calledPlayer, game->fullMap, game->goldMap);
        // update conflicting player
        player_moveUpAndDown(conflictingPlayer, -1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_moveLeftAndRight(conflictingPlayer, -1, grid_get(game->originalMap, player_getYLocation(conflictingPlayer), player_getXLocation(conflictingPlayer)));
        player_updateVisibility(conflictingPlayer, game->fullMap, game->goldMap);
        // update all
        game_updateAllUsers(game);
    }

    return false;
}

/* key press */

bool game_keyPress(game_t* game, addr_t player, const char* message) {

    player_t* currPlayer = roster_getPlayerFromAddr(game->players, player);
    if (!message_eqAddr(game->spectator, player) && (currPlayer == NULL)) {
        message_send(player, "ERROR Please start PLAY or SPECTATE first.");
        return false;
    }

    switch(message[4]) {        // message is in format "KEY k"
        case 'Q':
            return game_Q_quitGame(game, player, message);
        case 'h':
            return game_h_moveLeft(game, player, message);
        case 'l':
            return game_l_moveRight(game, player, message);
        case 'j':
            return game_j_moveDown(game, player, message);
        case 'k':
            return game_k_moveUp(game, player, message);
        case 'y':
            return game_y_moveDiagUpLeft(game, player, message);
        case 'u':
            return game_u_moveDiagUpRight(game, player, message);
        case 'b':
            return game_b_moveDiagDownLeft(game, player, message);
        case 'n':
            return game_n_moveDiagDownRight(game, player, message);
        default:
            message_send(player, "ERROR Invalid key.");
            return false;
    }

}

/* helper getter functions */

grid_t* game_returnFullMap(game_t* game) {
    return game->fullMap;
}

grid_t* game_returnGoldMap(game_t* game) {
    return game->goldMap;
}

int game_returnRemainingGold(game_t* game) {
    return game->remainingGold;
}