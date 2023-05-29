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

/**************** global integer ****************/
#define MAX_PLAYER_NAME_LENGTH 50
#define MAX_MSG_SIZE 1024

/**************** global types ****************/
typedef struct clientInfo {
    bool isPlayer;
    char playername[MAX_PLAYER_NAME_LENGTH];
    const char* playerID;
    addr_t serverAddr;
    WINDOW* clientwindow;
    int curX;  
    int curY;
} clientInfo_t;

/**************** global variables ****************/
clientInfo_t* clientInfo;
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
    clientInfo = calloc(1, sizeof(clientInfo_t));  // allocating memory for clientInfo
    parseArgs(argc, argv);
    initializeDisplay();
    initializeNetwork(argv[1], argv[2], stderr, clientInfo->isPlayer ? clientInfo->playername : NULL);
    exit(0);
}

/**************** parseArgs() ****************/
/* 
 */
static void parseArgs(const int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Invalid arguments; Usage: ./client hostname port [player_name]\n");
        exit(1);
    }
    else {
        if (argc == 4) {
            clientInfo->isPlayer = true;
            strncpy(clientInfo->playername, argv[3], MAX_PLAYER_NAME_LENGTH - 1);
            clientInfo->playername[MAX_PLAYER_NAME_LENGTH - 1] = '\0';
        }
        else {
            clientInfo->isPlayer = false;
        }
    }
}

void initializeDisplay() {
    //Start ncurses mode; create windoe
    WINDOW* newwin = initscr();
    clientInfo->clientwindow = newwin;

    // Allow keyboard mapping and don't display the key press 
    raw();
    noecho();

    //Create window for displaying the game
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    getmaxyx(clientInfo->clientwindow, clientInfo->curX, clientInfo->curY);

    if (clientInfo->isPlayer) {
        mvprintw(0, 0, "Player %s has 0 nuggets (211 nuggets unclaimed).", clientInfo->playername);
    } else {
        mvprintw(0, 0, "Spectator: 211 nuggets unclaimed.");
    }
    
    // Refresh the screen to display changes
    refresh();
}

void initializeNetwork(char* server, char* port, FILE* errorFile, char* playerName) {
    // For the client to server message
    playMessage = mem_malloc_assert(message_MaxBytes, "initializeNetwork(): Memory Message");

    // if name is NULL, client is a spectator. Otherwise, player
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

    if (!message_setAddr(server, portStr, &(clientInfo->serverAddr))) {
        fprintf(stderr, "Error: Invalid hostname or port number.\n");
        exit(2);
    }

    if (!message_isAddr(clientInfo->serverAddr)) {
        fprintf(stderr, "Error: Failed to setup server address.\n");
        exit(3);
    }

    // Initializes the message server
    if (message_init(errorFile) == 0) {
        // error occurred while initializing the client's connection
        fprintf(stderr, "Error: Unable to initialize client connection\n");
        exit(4);
    }

    // Join the server
    message_send(clientInfo->serverAddr, playMessage);

    // Handle input messages and loops until error occurs or exit is responsible for the bulk of server communication, handles input messages,
    if (!message_loop(NULL, 0, NULL, handleInput, handleMessage)) {
        fprintf(stderr, "Error: Failure while looping.\n");
        exit(5);
    }

    mem_free(playMessage);
}


/**************** handleMessage() ****************/
/* 
 */
static bool handleMessage(void* arg, const addr_t incoming, const char* message) {
    //Handle OK message 
    if (strncmp(message, "OK", 2) == 0) {
        char* ID = mem_malloc(5);
        sscanf(message, "OK %s", ID);
        clientInfo->playerID = ID;
        mem_free(ID);
    }
    // Handle GRID message
    else if (strncmp(message, "GRID", 4) == 0) {
        int nrows, ncols;
        sscanf(message, "GRID %d %d", &nrows, &ncols);
        if (clientInfo->curX < (nrows+1) || clientInfo->curY < (ncols+1)) {
            mvprintw(0,0,"Error: Display is not large enough for the grid. Please resize.\n");
            getmaxyx(clientInfo->clientwindow, clientInfo->curX, clientInfo->curY);
            move(0,0);
            return false;
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
        if (clientInfo->isPlayer) {
            mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed). GOLD received: %d", clientInfo->playername, p, r, n);
        }
        else {
            mvprintw(0, 0, "Spectator: %d nuggets unclaimed.", r);
        }
    }
    // Handle DISPLAY message
    else if (strncmp(message, "DISPLAY\n", 8) == 0) {
        char* gridString = (char*)malloc(MAX_MSG_SIZE * sizeof(char));
        if (gridString == NULL) {
            fprintf(stderr, "Error: Out of memory.\n");
            return false;
        }
        sscanf(message, "DISPLAY\n%[^\n]", gridString);
        wprintw(clientInfo->clientwindow, "%s", gridString);
        wrefresh(clientInfo->clientwindow);
        free(gridString);
    }
    // Handle DISPLAY message
    // else if (strncmp(message, "DISPLAY\n", 8) == 0) {
    //     char* gridString = (char*)malloc(MAX_MSG_SIZE * sizeof(char));
    //     if (gridString == NULL) {
    //         fprintf(stderr, "Error: Out of memory.\n");
    //         return false;
    //     }
    //     sscanf(message, "DISPLAY\n%[^\n]", gridString);

    //     // Clear the window for new grid
    //     wclear(clientInfo->clientwindow);

    //     // New functionality to replicate the display function
    //     if (gridString == NULL) {
    //         fprintf(stderr, "displayGrid(): NULL 'grid' passed\n");
    //         free(gridString);
    //         return false;
    //     }

    //     // Add new grid to display
    //     int i = 0;
    //     int x = 1;
    //     int y = 0;
    //     while (gridString[i] != '\0') {
    //         if (gridString[i] == '\n'){
    //             y++;
    //             x = 1;
    //         } else {
    //             mvwaddch(clientInfo->clientwindow, y, x, gridString[i]);
    //             x++;
    //         }
    //         i++;
    //     }

    //     wrefresh(clientInfo->clientwindow);
    //     free(gridString);
    // }
    // Handle QUIT message
    else if (strncmp(message, "QUIT", 4) == 0) {
        endwin();
        char* quitMessage = mem_malloc(strlen(message));
        strcpy(quitMessage, message);
        quitMessage = quitMessage + strlen("QUIT ");
        printf("%s\n", quitMessage);
        mem_free(quitMessage);
        message_done();

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
        if (clientInfo->isPlayer) {
            mvprintw(0, 0, "Player %s has unknown keystroke", clientInfo->playername);
        }
        else {
            mvprintw(0, 0, "Spectator: unknown keystroke");
        }
    }
    return false;
}


bool handleInput(void* arg){
    int key = getch();

    if(arg == NULL) {
        fprintf(stderr, "Error: Invalid keyboard input.\n");
        return true;
    }

    char* keyMessage = mem_malloc(10);
    
    if(key != 'Q'){ // Condition to check if key is not equal to 'Q'
        sprintf(keyMessage, "KEY %c", key);
        message_send(clientInfo->serverAddr, keyMessage);
        mem_free(keyMessage);
        return false;
    }
    else {
        message_send(clientInfo->serverAddr, "KEY Q");
        mem_free(keyMessage);
        return true;
    }

    clrtoeol();
}
