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
#include "common/grid.h"
#include "common/player.h"
#include "common/game.h"
#include "common/message.h"

/**************** function declarations ****************/

void parseArgs(const int argc, char* argv[]);
game_t* initializeGame();
bool handleMessage(void* arg);
/* message_loop(void* arg, const float timeout,
             bool (*handleTimeout)(void* arg),
             bool (*handleInput)  (void* arg),
             bool (*handleMessage)(void* arg,
                                   const addr_t from, const char* buf))*/
void game_over();

/**************** main ****************/

int main(const int argc, char* argv[]) {

    // Verify arguments and seed.
    parseArgs(argc, argv);
    game_t* game = initializeGame();

    // Initialize the network and announce the port number.
    int portID = message_init(stdin);

    // Wait for messages from clients (players or spectators). (call message_loop() from message)

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
game_t* initializeGame(char* mapFileName) {

    game_t* newGame = game_new(mapFileName);
    if (newGame == NULL) {
        fprintf(stderr, "Unable to create a new game from given map file.\n");
        exit(3);
    }

    game_setGold(newGame);

}

bool handleMessage(void* arg) {

    char* message = arg;    // cast to string

    switch (message[0]) {
        case "S": // spectator
        case "P": // new player
        case "K": // key press, switch again
    }
    
    /* Split message into a string array
     * split takes strncmp for the first command, then the rest of it is just information
     * or you can just strtok once for the first cmd
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
     *      QUIT You have been replaced by a new spectator.
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

}

/*
    In the game:
    PLAY adds a player to the Game hashtable, QUIT if game is full or no name provided, OK if successfully added
    to move a player, you need to
        -get the player from the hashtable
        -use cmds from player module
 */