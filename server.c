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
 *
 * Selena Zhou, Kyla Widodo, 23S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"
#include "player.h"

/**************** function declarations ****************/

void parseArgs(const int argc, char* argv[]);
void initializeGame();
void handleMessage();
void message_loop();
void game_over();

/**************** functions ****************/

int main(const int argc, char* argv[]) {

    parseArgs(argc, argv);

}

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


/**************** helper functions ****************/