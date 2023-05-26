/* 
 * server.c - Nuggets game server program.
 * 
 * server.c runs the game server that individual clients can connect to.
 * The server initializes the port, and then listens to connections from clients,
 * updating the master game and sending update info to each client.
 * 
 * Exit messages:
 * - (1): incorrect number of arguments
 * - (2): invalid argument
 * - (3): unable to create map grid
 *
 * Selena Zhou, Kyla Widodo, 23S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common/grid.h"
#include "common/player.h"
#include "common/game.h"
#include "support/message.h"

/**************** global variable ****************/

game_t* game;

/**************** function declarations ****************/

void parseArgs(const int argc, char* argv[]);
void initializeGame(char* mapFileName);
bool handleMessage(void* arg, const addr_t from, const char* message);
void game_over(); // calls message_done

/**************** main ****************/

int main(const int argc, char* argv[]) {

    // Verify arguments and seed.
    parseArgs(argc, argv);
    // initializeGame();

    // Initialize the network and announce the port number.
    int portID = message_init(stdin);
    fprintf(stdout, "Server is running at %d\n", portID);

    // Wait for messages from clients (players or spectators). (call message_loop() from message)
    message_loop(NULL, 0, NULL, handleInput, handleMessage);        // figure out the first three args

    // Free everything, call game_over()
    // game_over();

}

/**************** functions ****************/

// Validate arguments, exits nonzero if fails
void parseArgs(const int argc, char* argv[]) {

    if (argc < 2 || argc > 3) {     // incorrect number of arguments
        fprintf(stderr, "Usage: ./server mapFile.txt [seed]\n");
        exit(1);
    }

    FILE* fp = fopen(argv[1], "r");     // open map file
    if (fp == NULL) {
        fprintf(stderr, "Error: unable to open map file.\n");
        exit(2);
    }

    if (argc == 3) {    // create random seed if no seed provided, or validate provided seed
        int seed;
        if (sscanf(argv[2], "%d", &seed) != 1) {
            fprintf(stderr, "Error: seed must be an integer.\n");
            exit(2);
        }
        srand(seed);
    } else {
        srand(getpid());
    }

}

// Initializes game locally, makes sure everything can be set up
// void initializeGame(char* mapFileName) {

//     game = game_new(mapFileName);
//     if (game == NULL) {
//         fprintf(stderr, "Unable to create a new game from given map file.\n");
//         exit(3);
//     }

//     game_setGold(game);

// }

bool handleInput (void *arg) {
    if(feof(stdin)) {
        return true;
    } else {
        return false;
    }
}

bool handleMessage(void* arg, const addr_t from, const char* message) {
    if (strncmp(message, "PLAY", strlen("PLAY")) == 0) {
        message_send(from, "PLAY command received.");               // for debugging, delete later
        game_addPlayer(game, from, message);                        // new player
    }
    else if (strncmp(message, "SPECTATE", strlen("SPECTATE")) == 0) {
        message_send(from, "SPECTATE command received.");           // for debugging, delete later
        game_addSpectator(game, from);                              // new spectator
    }
    else if (strncmp(message, "KEY", strlen("KEY")) == 0) {
        message_send(from, "KEY command received.");                // for debugging, delete later
        return game_keyPress(game, from, message);                  // key press
    }
    else if (strncmp(message, "QUIT", strlen("QUIT")) == 0) {       // for debugging, delete this!
        message_send(from, "Server is quitting.");
        message_done();
        return true;
    }
    else {
        message_send(from, "Command not recognized.");
        // log error here?
    }
    return false;
}

