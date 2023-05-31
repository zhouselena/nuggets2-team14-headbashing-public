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
#include "game.h"

/**************** global types ****************/

typedef struct roster {
    set_t* players;         // key: playerID    value: player_t*
} roster_t;

typedef struct findPlayerPack {
    addr_t matchAddress;
    char matchPlayerID;
    player_t* foundPlayer;
} findPlayerPack_t;

/**************** file local helper functions ****************/
/* opaque to those outside of the file*/

/**************** roster_updateAllPlayers_Helper ****************/
/* To be passed into set_iterate for roster_updateAllPlayers.
 * Given a player, tells them to update their visible map based on new server map,
 * then sends that player new DISPLAY message to update their display.
 */
void roster_updateAllPlayers_Helper(void* arg, const char* key, void* item) {
    game_t* game = arg;
    player_t* currentPlayer = item;
    player_updateVisibility(currentPlayer, game_returnFullMap(game), game_returnGoldMap(game));

    grid_t* visibleGrid = player_getMap(currentPlayer);
    grid_t* visibleGold = player_getVisibleGold(currentPlayer);
    grid_overlay(visibleGrid, visibleGold, visibleGrid, visibleGrid);
    const char* gridString = grid_string(visibleGrid);
    char* sendDisplayMsg = malloc(strlen("DISPLAY") + strlen(gridString) + 5);
    sprintf(sendDisplayMsg, "DISPLAY\n%s", gridString);
    message_send(player_getAddr(currentPlayer), sendDisplayMsg);
    free(sendDisplayMsg);
}

/**************** roster_updateAllPlayersGold_Helper ****************/
/* To be passed into set_iterate for roster_updateAllPlayersGold.
 * Sends a GOLD update remaining gold message to given player.
 */
void roster_updateAllPlayersGold_Helper(void* arg, const char* key, void* item) {
    game_t* game = arg;
    player_t* currentPlayer = item;

    char* sendGoldMsg = malloc(20);
    sprintf(sendGoldMsg, "GOLD 0 %d %d", player_getGold(currentPlayer), game_returnRemainingGold(game));
    message_send(player_getAddr(currentPlayer), sendGoldMsg);
    free(sendGoldMsg);
}

/**************** roster_createGameMessage_Helper ****************/
/* To be passed into set_iterate for roster_createGameMessage.
 * Appends player name and purse to existing message.
 */
void roster_createGameMessage_Helper(void* arg, const char* key, void* item) {
    player_t* player = item;
    if (!message_isAddr(player_getAddr(player))) return; // If player exited before game over, don't print in summary

    char** playerSummary = arg;
    char* currentMsg = *playerSummary;
    char* newMsg = malloc(strlen(currentMsg) + 70);

    sprintf(newMsg, "%s\n%c %7d %s", currentMsg, player_getID(player), player_getGold(player), player_getName(player));
    free(currentMsg);

    *playerSummary = newMsg;
    arg = playerSummary;
}

/**************** roster_createGameMessage_sendHelper ****************/
/* To be passed into set_iterate for roster_createGameMessage.
 * Sends GAME OVER message to given player.
 */
void roster_createGameMessage_sendHelper(void* arg, const char* key, void* item) {
    char* message = arg;
    player_t* player = item;
    message_send(player_getAddr(player), message);
}

/**************** roster_delete_helper ****************/
void roster_delete_helper(void* item) {
    player_t* currPlayer = item;
    player_delete(currPlayer);
}

/**************** functions ****************/

/**************** roster_new ****************/
/* see roster.h for description */
roster_t* roster_new() {

    roster_t* roster = malloc(sizeof(roster_t));
    if (roster == NULL) return NULL;

    roster->players = set_new();
    return roster;

}

/**************** roster_new ****************/
/* see roster.h for description */
bool roster_addPlayer(roster_t* roster, player_t* player) {
    char playerID = player_getID(player);
    return set_insert(roster->players, &playerID, player);
}

/**************** roster_updateAllPlayers ****************/
/* see roster.h for description */
void roster_updateAllPlayers(roster_t* roster, game_t* game) {
    set_iterate(roster->players, game, roster_updateAllPlayers_Helper);
}

/**************** roster_updateAllPlayersGold ****************/
/* see roster.h for description */
void roster_updateAllPlayersGold(roster_t* roster, game_t* game) {
    set_iterate(roster->players, game, roster_updateAllPlayersGold_Helper);
}

/**************** roster_createGameMessage ****************/
/* see roster.h for description */
char* roster_createGameMessage(roster_t* roster) {
    int lineSize = 20 + 50;
    char* message = calloc(lineSize*26, sizeof(char));
    sprintf(message, "QUIT GAME OVER:");
    set_iterate(roster->players, &message, roster_createGameMessage_Helper);
    set_iterate(roster->players, message, roster_createGameMessage_sendHelper);
    return message;
}

/**************** roster_delete ****************/
/* see roster.h for description */
void roster_delete(roster_t* roster) {
    set_delete(roster->players, roster_delete_helper);
    free(roster);
}

/* get player from info functions */

/**************** roster_getPlayerFromAddr_Helper ****************/
/* To be passed into set_iterate for roster_getPlayerFromAddr.
 * Given a player, if it matches the address to be found, then add player to info pack.
 */
void roster_getPlayerFromAddr_Helper(void* arg, const char* key, void* item) {
    findPlayerPack_t* playerPack = arg;
    player_t* currPlayer = item;
    if (message_eqAddr(playerPack->matchAddress, player_getAddr(currPlayer))) {
        playerPack->foundPlayer = currPlayer;
    }
}

/**************** roster_getPlayerFromAddr ****************/
/* see roster.h for description */
player_t* roster_getPlayerFromAddr(roster_t* roster, addr_t playerAddr) {
    findPlayerPack_t* playerPack = malloc(sizeof(findPlayerPack_t));
    playerPack->matchAddress = playerAddr;
    playerPack->foundPlayer = NULL;
    set_iterate(roster->players, playerPack, *roster_getPlayerFromAddr_Helper);
    player_t* found = playerPack->foundPlayer;
    free(playerPack);
    return found;
}

/**************** roster_getPlayerFromAddr_Helper ****************/
/* To be passed into set_iterate for roster_getPlayerFromID.
 * Given a player, if it matches the ID to be found, then add player to info pack.
 */

void roster_getPlayerFromID_Helper(void* arg, const char* key, void* item) {
    findPlayerPack_t* playerPack = arg;
    player_t* currPlayer = item;
    if (player_getID(currPlayer) == playerPack->matchPlayerID) {
        playerPack->foundPlayer = currPlayer;
    }
}

/**************** roster_getPlayerFromID ****************/
/* see roster.h for description */
player_t* roster_getPlayerFromID(roster_t* roster, char playerID) {
    findPlayerPack_t* playerPack = malloc(sizeof(findPlayerPack_t));
    playerPack->matchPlayerID = playerID;
    playerPack->foundPlayer = NULL;
    set_iterate(roster->players, playerPack, *roster_getPlayerFromID_Helper);
    player_t* found = playerPack->foundPlayer;
    free(playerPack);
    return found;
}