/* 
 * client.c     team 14 - headbashing     May 20, 2023
 *
 * Handles the client side of the game. 
 * 
 * usage: ./client hostname port [playername]
 *   hostname is the IP address that the server's running on
 *   port is the number which the server listens for messages on
 *   playername, if provided, the client joins as a player
 *                 if not provided, the client joins as a view-only spectator
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include "common/mem.h"
#include <unistd.h>
#include "support/message.h"
#include "common/grid.h"
#include "common/player.h"
#include <ctype.h> // needed for isgraph() and isblank()


/**************** global integer ****************/
#define MAX_PLAYER_NAME_LENGTH 50
#define MAX_MSG_SIZE 1024

/**************** global types ****************/
typedef struct clientStruct {
    bool isPlayer;
    char playername[MAX_PLAYER_NAME_LENGTH];
    const char* playerID;
    addr_t serverAddr;
    WINDOW* clientwindow;
    int curX;  
    int curY;
} clientStruct_t;

/**************** global variables ****************/
clientStruct_t* clientStruct;
char* playMessage;

/**************** global functions ****************/
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[]);
void initializeDisplay();
void initializeNetwork(char* serverHost, char* port, FILE* errorFile, char* playerName);
static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t incoming, const char* message);


/**************** local functions ****************/

/**************** main ****************/
int main(const int argc, char* argv[]) {
    clientStruct = calloc(1, sizeof(clientStruct_t));
    if (!clientStruct) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }       
    parseArgs(argc, argv);
    initializeDisplay();
    initializeNetwork(argv[1], argv[2], stderr, clientStruct->isPlayer ? clientStruct->playername : NULL);
    exit(0);
}

/**************** parseArgs() ****************/
/* 
 */
static void parseArgs(const int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Invalid arguments; Usage: ./client hostname port [player_name]\n");
        exit(2);
    }
    else {
        if (argc == 4) {
            clientStruct->isPlayer = true;
            strncpy(clientStruct->playername, argv[3], MAX_PLAYER_NAME_LENGTH - 1);
            clientStruct->playername[MAX_PLAYER_NAME_LENGTH - 1] = '\0';
        }
        else {
            clientStruct->isPlayer = false;
        }
    }
}

void initializeDisplay() {
    //Start ncurses mode; create windoe
    WINDOW* newwin = initscr();
    clientStruct->clientwindow = newwin;

    // Allow keyboard mapping and don't display the key press 
    raw();
    noecho();

    //Create window for displaying the game
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    getmaxyx(clientStruct->clientwindow, clientStruct->curX, clientStruct->curY);
    
    // // Refresh the screen to display changes
    refresh();
}

void initializeNetwork(char* server, char* port, FILE* errorFile, char* playerName) {
    // For the client to server message
    char* playMessage = (char*)malloc(message_MaxBytes * sizeof(char));
    if (playMessage == NULL) {
        fprintf(stderr, "initializeNetwork(): Memory Message\n");
        exit(3);
    }

    // if name is NULL or empty, client is a spectator. Otherwise, player
    if (playerName == NULL) {
        sprintf(playMessage, "SPECTATE"); //construct spectate message
    }
    else {
        sprintf(playMessage, "PLAY %s", playerName); //construct play message
    }


    //Convert port to string
    char portStr[10];
    sprintf(portStr, "%d", atoi(port));
    fprintf(stderr, "Got: %s\n", portStr);

    if (!message_setAddr(server, portStr, &(clientStruct->serverAddr))) {
        fprintf(stderr, "Error: Invalid hostname or port number.\n");
        exit(4);
    }

    if (!message_isAddr(clientStruct->serverAddr)) {
        fprintf(stderr, "Error: Failed to setup server address.\n");
        exit(5);
    }

    // Initializes the message server
    if (message_init(errorFile) == 0) {
        // error occurred while initializing the client's connection
        fprintf(stderr, "Error: Unable to initialize client connection\n");
        exit(6);
    }

    // Join the server
    message_send(clientStruct->serverAddr, playMessage);

    // Loop, waiting for input or for messages; provide callback functions.
    // We use the 'arg' parameter to carry a pointer to 'server'.
    bool ok = message_loop(&(clientStruct->serverAddr), 0, NULL, handleInput, handleMessage);

    // Shut down the message module
    message_done();
    
    mem_free(playMessage);
    if (!ok) {
        fprintf(stderr, "Error: message_loop failed.\n");
        // Clean up and terminate the program if necessary
        exit(1);
    }
}

/**************** handleMessage() ****************/
/* 
 */
static bool handleMessage(void* arg, const addr_t incoming, const char* message) {
    //Handle OK message 
    if (strncmp(message, "OK", 2) == 0) {
        char* ID = mem_malloc(5);
        sscanf(message, "OK %s", ID);
        clientStruct->playerID = ID;
        mem_free(ID);
    }
    // Handle GRID message
    else if (strncmp(message, "GRID", 4) == 0) {
        int nrows, ncols;
        if(sscanf(message, "GRID %d %d", &nrows, &ncols) != 2) {
            fprintf(stderr, "Error: Cannot parse GRID message.\n");
            return false;
        }
        while (clientStruct->curX < (nrows+1) || clientStruct->curY < (ncols+1)) {
            mvprintw(0,0,"Error: Display is not large enough for the grid. Please resize.\n");
            // Wait for the user to resize the window.
            while (clientStruct->curX < (nrows+1) || clientStruct->curY < (ncols+1)) {
                getmaxyx(clientStruct->clientwindow, clientStruct->curX, clientStruct->curY);
            }
            move(0,0);
        }
    }
    // Handle GOLD message
    else if (strncmp(message, "GOLD", 4) == 0) {
        int n; //inform player it has collected n nuggets
        int p; // purse has p gold nugs
        int r; // r gold left to be found
        int args = sscanf(message, "GOLD %d %d %d", &n, &p, &r);

        // Check number of args
        if (args != 3) {
            fprintf(stderr, "ERROR: invalid number of arguments passed\n");
            return false;
        }
        // Update status line
        if (clientStruct->isPlayer) {
            mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed). GOLD received: %d", clientStruct->playername, p, r, n);
        }
        else {
            mvprintw(0, 0, "Spectator: %d nuggets unclaimed.", r);
        }
    }
    // Handle DISPLAY message
    else if (strncmp(message, "DISPLAY", 7) == 0) {
        // Split the message into lines
        char* line = strtok(message, "\n");
        int lineNumber = 1;
        while (line != NULL) {
            // Skip the "DISPLAY" line
            if (strncmp(line, "DISPLAY", 7) != 0) {
                // Print each line of the grid
                mvprintw(lineNumber++, 0, "%s", line);
            }
            // Get the next line
            line = strtok(NULL, "\n");
        }
        wrefresh(clientStruct->clientwindow);
    }
    // Handle QUIT message
    else if (strncmp(message, "QUIT", 4) == 0) {
        endwin();
        if (strlen(message) < strlen("QUIT ")) {
            fprintf(stderr, "Error: QUIT message is too short.\n");
            return false;
            }

        char* quitMessage = mem_malloc(strlen(message) + 1);  // +1 for the null-terminator
        if (!quitMessage) {
            fprintf(stderr, "Error: Memory allocation for quitMessage failed.\n");
            return false;
        }

        strcpy(quitMessage, message);

        char* quitMessageContent = quitMessage + strlen("QUIT ");
        printf("%s\n", quitMessageContent);

        mem_free(quitMessage);  // now it's safe to free quitMessage
        message_done();
        return true; // Stop processing further messages
    }
    // Handle ERROR message
    else if (strncmp(message, "ERROR", 5) == 0) {
        char errorMsg[MAX_MSG_SIZE];
        sscanf(message, "ERROR %[^\n]", errorMsg);
        fprintf(stderr, "Error: %s\n", errorMsg);
    }
    // Unknown message
    else {
        char errorMsg[MAX_MSG_SIZE];
        sscanf(message, "%[^\n]", errorMsg);
        fprintf(stderr, "Unknown message: %s\n", errorMsg);
        if (clientStruct->isPlayer) {
            mvprintw(0, 0, "Player %s has unknown keystroke", clientStruct->playername);
        }
        else {
            mvprintw(0, 0, "Spectator: unknown keystroke");
        }
    }
    return false;
}

static bool handleInput(void* arg) {
    char* keySend = mem_malloc(10);
    char* keyChar = arg;

    if(sscanf(keyChar, "%c", keySend) == 1){
        sprintf(keySend, "KEY %s", keyChar);
        message_send(clientStruct->serverAddr, keySend);
        if(*keySend == 'Q' || *keySend == 'q'){  // Check if Q/q is pressed
            mem_free(keySend);
            delwin(clientStruct->clientwindow);  // Delete the window
            endwin();  // End ncurses mode
            exit(0);  // This will close the program when Q is pressed
        }
        mem_free(keySend);
        return true;
    }
    else{
        message_send(clientStruct->serverAddr, "KEY Q");
        mem_free(keySend);
        return false;
    }

    clrtoeol();
}


