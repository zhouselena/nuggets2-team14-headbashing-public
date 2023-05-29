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
#include "mem.h"
#include <unistd.h>
#include "message.h"
#include "grid.h"
#include "player.h"

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
void quitClient(const char* reason);


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
        char* hostname = argv[1];
        char* port = argv[2];

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
    //Start ncurses mode 
    initscr();

    // Allow keyboard mapping and don't display the key press 
    raw();
    noecho();

    //Create window for displaying the game
    clientInfo->clientwindow = newwin(NR + 1, NC + 1, 0, 0);
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(1));

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
        quitClient("Hostname or Portnumber invalid");
        exit(2);
    }

    if (!message_isAddr(clientInfo->serverAddr)) {
        fprintf(stderr, "Error: Failed to setup server address.\n");
        quitClient("Connection failed");
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

    message_done();
    mem_free(playMessage);
}


/**************** handleMessage() ****************/
/* 
 */
static bool handleMessage(void* arg, const addr_t incoming, const char* message) {
    //Handle OK message 
    if (strncmp(message, "OK", strlen("OK ")) == 0) {
        char* ID = mem_malloc(5);
        sscanf(message, "OK %s", ID);
        clientInfo->playerID = ID;
        mem_free(ID);
    }
    // Handle GRID message
    else if (strncmp(message, "GRID", strlen("GRID ")) == 0) {
        int nrows, ncols;
        sscanf(message, "GRID %d %d", &nrows, &ncols);
        if (nrows > NR || ncols > NC) {
            fprintf(stderr, "Error: Display is not large enough for the grid. Please resize.\n");
            return false;
        }
    }
    // Handle GOLD message
    else if (strncmp(message, "GOLD", strlen("GOLD ")) == 0) {
        int n; //inform player it has collected n nuggets
        int p; // purse has p gold nugs
        int r; // r gold left to be found
        sscanf(message, "GOLD %d %d %d", &n, &p, &r);

        // Update status line
        if (clientInfo->isPlayer) {
            mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed). GOLD received: %d", clientInfo->playername, p, r, n);
        }
        else {
            mvprintw(0, 0, "Spectator: %d nuggets unclaimed.", r);
        }
    }
    // Handle DISPLAY message
    else if (strncmp(message, "DISPLAY\n", strlen("DISPLAY ")) == 0) {
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
    // Handle QUIT message
    else if (strncmp(message, "QUIT", strlen("QUIT ")) == 0) {
        endwin();
        char reason[MAX_MSG_SIZE];
        sscanf(message, "QUIT %[^\n]", reason);
        quitClient(reason);
        return false;
    }
    // Handle ERROR message
    else if (strncmp(message, "ERROR", strlen("ERROR ")) == 0) {
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
    refresh();
    return true;
}


bool handleInput(void* arg){
    int key = getch(); // ncurses call to get the key press

    if(arg == NULL) {
        fprintf(stderr, "Error: Invalid keyboard input.\n");
        return false;
    }

    char* keyMessage = mem_malloc(10);
    
    if (key == 'Q' || key == EOF){
        sprintf(keyMessage, "KEY Q");
        message_send(clientInfo->serverAddr, keyMessage);
        quitClient("User quit the game."); // quit game if reach EOF or 'Q' on stdin
    }
    else if(clientInfo->isPlayer){
        if (strchr("hljkyubnHLJKYUBN", key)){
            sprintf(keyMessage, "KEY %c", key);
            message_send(clientInfo->serverAddr, keyMessage);
        }
        else{
            fprintf(stderr, "Error: Invalid player keystroke.\n");
            return false;
        }
    }
    else{
        fprintf(stderr, "Invalid spectator keystroke.\n");
        return false;
    }

    mem_free(keyMessage);
    return true;
}

void quitClient(const char* reason){
    fprintf(stderr, "Game Over: %s\n", reason);
    mem_free(clientInfo);
    exit(0);
}
