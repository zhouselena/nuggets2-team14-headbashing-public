/* 
 * game.h - header file for Nuggets 'game' module
 * 
 * A 'game' is the server hosting the overall nuggets game.
 * The game holds information about all the players/clients, the map being used,
 * the over number of gold available, and spectator information.
 *
 * Selena Zhou, Kyla Widodo, 23S
 */

#ifndef __GAME_H
#define __GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"
#include "../support/message.h"

/**************** global types ****************/
typedef struct game game_t;

/**************** functions ****************/

/* create and delete */

/**************** game_new ****************/
/* Allocates memory for new game, initializes map, map info, and players.
 * Also sets gold in map.
 *
 * Caller provides: valid map file path
 * Returns: initialized game or NULL upon failure.
 */
game_t* game_new(char* mapFileName);

/**************** end_game ****************/
/* To be called once remaining gold becomes 0. Sends a GAME OVER summary to all clients.
 *
 * Caller provides: valid game
 * Returns: nothing.
 */
void end_game(game_t* game);

/* receive input */

/**************** game_addPlayer ****************/
/* Called when server receives PLAY from client. Adds a player to the game if the game not full.
 * Gets a random location for player, sends OK/GRID/GOLD/DISPLAY to new player, 
 * then updates all users with the updated map. If user spawns on a gold pile, they receive that gold pile.
 *
 * Caller provides: valid game, player address, 'PLAY playername' message
 * Returns: nothing.
 */
void game_addPlayer(game_t* game, addr_t playerAddr, const char* message);

/**************** game_addSpectator ****************/
/* Called when server receives SPECTATE from client. Adds spectator and kicks previous spectator out.
 * Sends all display information (GRID/GOLD/DISPLAY) to client.
 *
 * Caller provides: valid game, spectator address
 * Returns: nothing.
 */
void game_addSpectator(game_t* game, addr_t newSpectator);

/* key press functions */

/**************** game_Q_quitGame ****************/
/* Called when server receives 'KEY Q' from client.
 * If address received is the spectator, sends spectator QUIT message and removes spectator from game.
 * If address received is a player, sends player QUIT message and removes player from the game,
 * removes player icon from map and updates all existing users.
 *
 * Caller provides: valid game, player address, 'KEY' message
 * Returns: false
 */
bool game_Q_quitGame(game_t* game, addr_t player, const char* message);

/**************** game_[KEY]_move[DIRECTION] ****************/
/* Called when server receives 'KEY [hljkyubn]' from client.
 * If address received is the spectator, sends spectator ERROR message.
 * Checks if player can move. If they can, then update everyone's display.
 * If they moved to a gold point, call found gold function to update total gold count.
 * If they move to an existing player's position, swaps the two player positions.
 *
 * Caller provides: valid game, player address, 'KEY' message
 * Returns: true if found gold returns true (game over), false otherwise
 */
bool game_h_moveLeft(game_t* game, addr_t player, const char* message);
bool game_l_moveRight(game_t* game, addr_t player, const char* message);
bool game_j_moveDown(game_t* game, addr_t player, const char* message);
bool game_k_moveUp(game_t* game, addr_t player, const char* message);
bool game_y_moveDiagUpLeft(game_t* game, addr_t player, const char* message);
bool game_u_moveDiagUpRight(game_t* game, addr_t player, const char* message);
bool game_b_moveDiagDownLeft(game_t* game, addr_t player, const char* message);
bool game_n_moveDiagDownRight(game_t* game, addr_t player, const char* message);

/**************** game_[CAPITALKEY]_move[DIRECTION] ****************/
/* Called when server receives 'KEY [HLJKYUBN]' from client.
 * Will keep calling game_[KEY]_move[DIRECTION] function until can't move anymore.
 * Returns what is returned from game_[KEY]_move[DIRECTION].
 */

bool game_H_moveLeft(game_t* game, addr_t player, const char* message);
bool game_L_moveRight(game_t* game, addr_t player, const char* message);
bool game_J_moveDown(game_t* game, addr_t player, const char* message);
bool game_K_moveUp(game_t* game, addr_t player, const char* message);
bool game_Y_moveDiagUpLeft(game_t* game, addr_t player, const char* message);
bool game_U_moveDiagUpRight(game_t* game, addr_t player, const char* message);
bool game_B_moveDiagDownLeft(game_t* game, addr_t player, const char* message);
bool game_N_moveDiagDownRight(game_t* game, addr_t player, const char* message);

/**************** game_keyPress ****************/
/* Called when server receives 'KEY [keystroke]' from client.
 * Calls individual key press functions depending on keystroke, or ERROR message if invalid keystroke.
 *
 * Caller provides: valid game, player address, 'KEY' message
 * Returns: true if found gold returns true (game over), false otherwise
 */
bool game_keyPress(game_t* game, addr_t player, const char* message);

/* getters */

/**************** game_returnFullMap ****************/
/* Given valid game, returns map with all player ID's.
 */
grid_t* game_returnFullMap(game_t* game);
/**************** game_returnGoldMap ****************/
/* Given valid game, returns map with all uncollected gold locations.
 */
grid_t* game_returnGoldMap(game_t* game);
/**************** game_returnRemainingGold ****************/
/* Given valid game, returns how much gold is left.
 */
int game_returnRemainingGold(game_t* game);

#endif // __GAME_H
