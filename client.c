/* 
 *
 * Handles the client side of the game. 
 * 
 * server.c runs the game server that individual clients can connect to.
 * Usage: ./client hostname port [playername]
 *   hostname = IP address that the server's running on
 *   port = number which the server listens for messages on
 *   playername, if provided = client joins as a player
 *               if not provided = client joins as a view-only spectator
 * 
 * Selena Zhou, Kyla Widodo, 23S
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

/**************** global types ****************/
typedef struct clientStruct {
    bool isPlayer;
    char playername[MAX_PLAYER_NAME_LENGTH];
    char* playerID;
    addr_t serverAddr;
    WINDOW* clientwindow;
    int curX;  
    int curY;
    int goldNuggets; 
    int totalNuggets;
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

/**************** main ****************/
/*
* Main function of client side of game. Allocates memory for the clientStruct and calls other functions to parse command line arguments, initialize game display, and set up network communication. 
* 
* Caller provides: Command-line arguments argc and argv[].
* Returns: An integer representing the exit status of the program.
*/
int main(const int argc, char* argv[]) {
    clientStruct = calloc(1, sizeof(clientStruct_t)); //Allocate memory of data struct
    if (!clientStruct) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }    

    parseArgs(argc, argv); //Parse the incoming arguments
    initializeDisplay(); //Initialize the ncurses and window
    initializeNetwork(argv[1], argv[2], stderr, clientStruct->isPlayer ? clientStruct->playername : NULL); //Initialize the connection, message_send, and message_loop
    
    // Shutting down program
    delwin(clientStruct->clientwindow);  //Delete the window
    endwin(); //end ncurses
    
    //Freeing memory
    mem_free(clientStruct->playerID);
    mem_free(clientStruct->playername);
    mem_free(clientStruct);

    exit(0);
}

/**************** parseArgs() ****************/
/*
 * Checks the validity of command-line arguments and initializes the clientStruct accordingly. 
 * If a player name is provided, the client is set to be a player; otherwise, it is set to be a spectator.
 *
 * Caller provides: Command-line arguments argc and argv[].
 * Returns: Nothing.
 */
static void parseArgs(const int argc, char* argv[]) {
    if (argc < 3 || argc > 4) { //If the usage is incorect
        fprintf(stderr, "Invalid arguments; Usage: ./client hostname port [player_name]\n");
        exit(2); //Exit program
    }
    else {
        if (argc == 4) { //If arguments is player, boolean true
            clientStruct->isPlayer = true; 
            strncpy(clientStruct->playername, argv[3], MAX_PLAYER_NAME_LENGTH - 1);
            clientStruct->playername[MAX_PLAYER_NAME_LENGTH - 1] = '\0';
        }
        else {
            clientStruct->isPlayer = false; //if not, spectator
        }
    }
}


/**************** initializeDisplay() ****************/
/*
 * Initializes display window for the game using the ncurses library.
 * Includes starting ncurses mode, creating a window, setting keyboard mapping, and setting up color pairs for the display
 *
 * Caller provides: Nothing
 * Returns: Nothing
 */
void initializeDisplay() {
    //Start ncurses mode; create window
    WINDOW* newwin = initscr();
    clientStruct->clientwindow = newwin;

    // Allow keyboard mapping and don't display the key press 
    raw();
    noecho();

    //Create window for displaying the game
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK); // color of the text and background
    attron(COLOR_PAIR(1));
    getmaxyx(clientStruct->clientwindow, clientStruct->curY, clientStruct->curX); //set the clients window size
    
    // // Refresh the screen to display changes
    refresh();
}

/**************** initializeDisplay() ****************/
/*
 * Set up network communication for client. Constructs initial message to the server, sets up server address, and send initial message to the server
 * Starts the message loop. Exit non-zero if any of these set ups and loops fail.

 * Caller provides: Server hostname serverHost, port number port, and player name playerName (optional).
 * Returns: Nothing
 */
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

    if (!message_setAddr(server, portStr, &(clientStruct->serverAddr))) { //Check if hostname and port number are correct
        fprintf(stderr, "Error: Invalid hostname or port number.\n");
        free(playMessage);
        exit(4);
    }

    if (!message_isAddr(clientStruct->serverAddr)) {
        fprintf(stderr, "Error: Failed to setup server address.\n"); //Check of serverAddress is correct
        free(playMessage);
        exit(5);
    }

    // Initializes the message server
    if (message_init(errorFile) == 0) {
        // error occurred while initializing the client's connection
        fprintf(stderr, "Error: Unable to initialize client connection\n");
        free(playMessage);
        exit(6);
    }

    // Join the server
    message_send(clientStruct->serverAddr, playMessage);
    mem_free(playMessage);

    // Loop, waiting for input or for messages; provide callback functions.
    // We use the 'arg' parameter to carry a pointer to 'server'.
    bool ok = message_loop(&(clientStruct->serverAddr), 0, NULL, handleInput, handleMessage);

    // Shut down the message module
    message_done();
    
    if (!ok) {
        fprintf(stderr, "Error: message_loop failed.\n");
        exit(1);
    }

}

/**************** handleMessage() ****************/
/* 
 * Handles the messages received from the server.
 * Parses and responds to keyword: "OK", "GRID", "GOLD", "DISPLAY", "QUIT", "ERROR"
 *
 * Returns: boolean value indicating whether to continue processing messages.
 */
static bool handleMessage(void* arg, const addr_t incoming, const char* message) {
    //Handle OK message 
    if (strncmp(message, "OK", 2) == 0) {
        char* ID = mem_malloc(5);  // Allocate an extra byte for the null terminator.
        sscanf(message, "OK %1s", ID);  // Read at most 1 characters into ID.
        ID[1] = '\0';  // Explicitly null-terminate the string.
        clientStruct->playerID = ID;
    }
    // Handle GRID message
    else if (strncmp(message, "GRID", 4) == 0) {
        int nrows, ncols;
        if(sscanf(message, "GRID %d %d", &nrows, &ncols) != 2) {
            fprintf(stderr, "Error: Cannot parse GRID message.\n");
            return false;
        }
        while (clientStruct->curY < (nrows+1) || clientStruct->curX < (ncols+1)) { // While the window is too small
            mvprintw(0,0,"Error: Display is not large enough for the grid. Please resize and press enter.\n");  // Let client know 
            mvprintw(2,0,"Your window must be at least %d wide and %d tall.\n", ncols+1, nrows+1);
            refresh();
            // Wait for the user to resize the window.
            while (getch() != '\n') { // If they press enter
                getmaxyx(clientStruct->clientwindow, clientStruct->curY, clientStruct->curX); //Get the new size
            }
        }
        move(0, 0);      // Move to where error line starts
        clrtoeol();      // Clear line
        move(2, 0);      // Move to where 2nd line starts 
        clrtoeol();      // Clear line
        refresh();       // Refresh the window and continue 
    }
    // EXTRA CREDIT: Handle GOLDSTEAL message
    else if (strncmp(message, "GOLDSTEAL", strlen("GOLDSTEAL")) == 0) {
        int n; // change of n nugs, if n < 0 then they are the victim
        int p; // purse has p gold nugs
        int r; // r gold left to be found
        char otherPlayID; // other conflicting player
        sscanf(message, "GOLDSTEAL %d %d %d %c", &n, &p, &r, &otherPlayID);

        // Update the client's gold nuggets
        clientStruct->goldNuggets = p;
        clientStruct->totalNuggets = r;
        
        // Update status line
        if (clientStruct->isPlayer) {
            if (n < 0) {
                mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed). Player '%c' stole %d from you!", clientStruct->playerID, p, r, otherPlayID, n*(-1));  
            }
            else if (n > 0) {
                mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed). You stole %d from player '%c'!", clientStruct->playerID, p, r, n, otherPlayID);
            }
            else {
                mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed). Player '%c' is too poor!", clientStruct->playerID, p, r, otherPlayID);
            }
            refresh();
        }
        else {
            mvprintw(0, 0, "Spectator: %3d nuggets unclaimed.", r);
        }
        
        // Refresh the screen to display changes
        refresh();
    }
    // Handle GOLD message
    else if (strncmp(message, "GOLD", 4) == 0) {
        int n; //inform player it has collected n nuggets
        int p; // purse has p gold nugs
        int r; // r gold left to be found
        sscanf(message, "GOLD %d %d %d", &n, &p, &r);

        // Update the client's gold nuggets
        clientStruct->goldNuggets = p;
        clientStruct->totalNuggets = r;
        
        // Update status line
        if (clientStruct->isPlayer) {
            if (n == 0) {
                mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed).", clientStruct->playerID, p, r);
            }
            else {
                mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed). GOLD received: %d", clientStruct->playerID, p, r, n);
            }
            refresh();
        }
        else {
            mvprintw(0, 0, "Spectator: %3d nuggets unclaimed.", r);
        }
        
        // Refresh the screen to display changes
        refresh();
    }
    // Handle DISPLAY message
    else if (strncmp(message, "DISPLAY", 7) == 0) {
        int lineNumber = 1; // Start from the first character after "DISPLAY\n"
        const char* line = message + 8;  // Skip "DISPLAY\n"

        while (line) { //Print the whole map
            const char* next_newline = strchr(line, '\n'); //find next line

            // If a newline found
            if (next_newline) {
                int line_length = next_newline - line; // Calculate length of line
                char* line_buffer = (char*)malloc(line_length + 1); // Allocate mem for line
                strncpy(line_buffer, line, line_length); // Copy line
                line_buffer[line_length] = '\0'; //Null-terminate

                // Print the line
                mvprintw(lineNumber++, 0, "%s", line_buffer);

                // Free the memory for the line buffer
                free(line_buffer);

                // Move to the start of the next line
                line = next_newline + 1;
            } else {
                // No more newlines - print the remaining part of the message
                mvprintw(lineNumber++, 0, "%s", line);
                line = NULL;  // Exit the loop
            }
        }

        // Refresh the screen
        wrefresh(clientStruct->clientwindow);
    }
    // Handle QUIT message
    else if (strncmp(message, "QUIT", 4) == 0) {
        delwin(clientStruct->clientwindow); // Delete the window
        endwin(); // end ncurser

        char* quitMessage = mem_malloc(strlen(message) + 1);  // +1 for null-terminator
        strcpy(quitMessage, message); //Copy the message

        printf("%s\n", quitMessage); //Print message

        mem_free(quitMessage);
        mem_free(clientStruct->playerID);
        mem_free(clientStruct);
        message_done(); //ends the message loop
        exit(0);  // Stop processing further messages -- exits the prgram
    }
    // Handle ERROR message
    else if (strncmp(message, "ERROR", 5) == 0) {
        fprintf(stderr, "Error: %s\n", message);
        // Update the status line with the error message
        if (clientStruct->isPlayer) {
            mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed). %s", clientStruct->playerID, clientStruct->goldNuggets, clientStruct->totalNuggets, message);
            refresh();
        } else {
            mvprintw(0, 0, "Spectator: %s", message);
            refresh();
        }
    }
    // Unknown/ Misordered message
    else {
        fprintf(stderr, "Unknown message: %s\n", message);

        // Update the status line with the unknown message
        if (clientStruct->isPlayer) {
            mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed). Unknown message: %s", clientStruct->playerID, clientStruct->goldNuggets, clientStruct->totalNuggets, message);
            refresh();
        } else {
            mvprintw(0, 0, "Spectator: Unknown message: %s", message);
            refresh();
        }
    }
    return false;
}

/**************** handleInput() ****************/
/* 
 * Handles user key presses. Function reads the key pressed by the user and sends a corresponding message to the server
 * 
 * Caller provides: void pointer arg (can be NULL)
 * Returns: boolean value to continue input loop.
 */
static bool handleInput(void* arg) {
    // Allocate a buffer for the key press.
    char* keySend = mem_malloc(10);

    // Read the key press. The ncurses function getch() is used to get the key pressed
    int keyChar = getch();

    // Check if Q is pressed.
    if(keyChar == 'Q'){
        sprintf(keySend, "KEY %c", keyChar);
        message_send(clientStruct->serverAddr, keySend);
        mem_free(keySend);
    }
    else { // If any other key is pressed
        sprintf(keySend, "KEY %c", keyChar);
        message_send(clientStruct->serverAddr, keySend);
        mem_free(keySend); 
    }

    // Reset status line to original message
    if (clientStruct->isPlayer) {
        mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed).", clientStruct->playerID, clientStruct->goldNuggets, clientStruct->totalNuggets);
    } else {
        mvprintw(0, 0, "Spectator: %d nuggets unclaimed.", clientStruct->totalNuggets);
    }
      
    clrtoeol(); // Clear

    return false;  // Return false to continue the input loop.
}