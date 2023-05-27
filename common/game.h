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
#include "../support/message.h"

/**************** global types ****************/
typedef struct game game_t;
typedef struct findPlayer findPlayer_t;

/**************** functions ****************/

game_t* game_new(char* mapFileName);

void game_addPlayer(game_t* game, addr_t playerAddr, const char* message);
void game_addSpectator(game_t* game, addr_t newSpectator);

void game_Q_quitGame(game_t* game, addr_t player, const char* message);
void game_h_moveLeft(game_t* game, addr_t player, const char* message);
void game_l_moveRight(game_t* game, addr_t player, const char* message);
void game_j_moveDown(game_t* game, addr_t player, const char* message);
void game_k_moveUp(game_t* game, addr_t player, const char* message);
void game_y_moveDiagUpLeft(game_t* game, addr_t player, const char* message);
void game_u_moveDiagUpRight(game_t* game, addr_t player, const char* message);
void game_b_moveDiagDownLeft(game_t* game, addr_t player, const char* message);
void game_n_moveDiagDownRight(game_t* game, addr_t player, const char* message);

void game_keyPress(game_t* game, addr_t player, char* message);

#endif // __GAME_H
