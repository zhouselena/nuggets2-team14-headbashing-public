# CS50 Nuggets
## Implementation Spec
### Team 14 - Headbashing, Spring 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes `game`, `player`, `roster`, and `gold` modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

As we are a team of 2, we were provided with `grid` so this will not be covered in our implementation spec.

## Plan for division of labor

Selena: `server` and modules required by server (`game`, `player`, `roster`,`gold`)

Kyla: `client` and modules required by client (`clientStruct`), scrum

Both: `DESIGN.md`, `IMPLEMENTATION.md`, `README.md`, `.gitignore` Makefiles, testing

## Player/Client

### Data structures

Client module uses one main structure that is `clientStruct`.

```
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
```

- isPlayer — boolean that returns true if the client is a player, returns false if spectator
- playername — holds the name of the player, as a string of characters. It will be used for identification in the game
- playerID — stores the unique identifier of the player in the game.
- serverAddr — contains the address of the server that the client is connected to
- clientwindow — is a pointer to a WINDOW structure. This represents the main window or screen that the client is using to interact with the game.
- curX - indicates the current X position of the player in the game's grid
- curY - indicates the current Y position of the player in the game's grid
- goldNuggets — represents the current amount of gold play collected
- totalNuggets — represents the total number of gold nuggets in the game

### Definition of function prototypes

```c
int main(const int argc, char* argv[]);
```

Initializes client structure, parses command-line arguments, initializes display, sets up network connection, enters game loop, and performs cleanup when the game is over.

```c
static void parseArgs(const int argc, char* argv[]);
```

Checks the validity of command-line arguments and initializes the client structure accordingly.

```c
void initializeDisplay();
```
Sets up the game display.

```c
void initializeNetwork(char* serverHost, char* port, FILE* errorFile, char* playerName);
```
Sets up the network connection, sends the initial message to the server, and starts the message loop.

```c
static bool handleMessage(void* arg, const addr_t incoming, const char* message);
```
Handles messages received from the server, updates the game state, and refreshes the display.

```c
static bool handleInput(void* arg);
```
Handles user key presses and sends corresponding messages to the server.

### Detailed pseudo code

#### `main`:  

    Allocate memory for client structure
	if memory allocation fails, display error message and exit
    calls parse args
    call initialize display
	call initialize network
		enter the game loop
			process server messages or handle the user inputs
    at the end of the program:
	delete the window
	end ncurses
    free allocated memory
    end game

#### `parseArgs`:

	if number of arguments incorrect
		display error message about invalid argument 
	check the validity of command-line arguments
	if theres a 4th argument
		set client as player
	if no 4th argument
		set client as spectator

#### `initializeDisplay`:

    Start ncurses mode
	create window
    Set keyboard mapping and dont display the key press 
    Create window for displaying the game
    Refresh the screen to display changes

#### `initializeNetwork`:

	Allocate memory for message to server
		if fail, exit
    if player name is give, construct a message
	if not client is spectator, construct message

    Convert port to string
    set up server details using hostname and port
    Initialize the message server
		if fail, error, free mem
	Send initial message to server
    Start message loop

#### `handleMessage`:

	Handle OK message
		extract playerID and store
	Handle GRID message
		extract grid dimensions 
		check if window size is big enough
			if not, client to resize
			when resized, update display
	Handle GOLD message or GOLDSTEAL
		extract relevant info
		update client gold nuggets and status
		refresh
	Handle DISPLAY message
		get map and display it
		refresh
	Handle QUIT message
		delete the window, end ncurses, free memory
		quit message
		stop program
	Handle ERROR message
		display message
	Handle Unknown/Misordered message

#### `handleInput`:

	read the key pressed by the user 
	if input recognized
		send a corresponding message to the server
	continue processing

---

## Server

### Data structures

As described in `DESIGN.md`, server uses `game`, `player`, `roster`, and `gold` modules.

### Definition of function prototypes

```c
int main(const int argc, char* argv[]);
```
Calls all functions, opens port, and prints to stdout.

```c
void parseArgs(const int argc, char* argv[]);
```
Parses and validates the command-line arguments. It exits with a nonzero status if the arguments are incorrect or invalid.

```c
void initializeGame(char* mapFileName)
```
This function initializes the game locally, creating the game/grid from the provided map file and setting up random gold piles.

```c
bool handleInput (void *arg)
```
This function handles input from stdin and is used to detect EOF.

```c
bool handleMessage(void* arg, const addr_t from, const char* message)
```
This function is responsible for handling messages from clients. It calls appropriate game functions based on the input message from a client.

```c
void game_over()
```
This function frees all information the game holds and shuts down the server gracefully by calling `message_done()`.

### Detailed pseudo code

#### main():

    calls parseArgs and initializeGame
    initializes port and prints out the portID
    loops through messages
    once game ends, call game_over to free everything

#### parseArgs():

    checks for correct number of arguments
    checks if map file can be opened
    if seed is provided, check that it is an integer
    otherwise generate random seed
    exit nonzero upon fail check

#### initializeGame():

    call game_new() to see if new game can be made
    exit nonzero upon failure

#### handleInput():

    if stdin is EOF, return true
    else return false

#### handleMessage():

    if message is PLAY, add player to game
    if message is SPECTATE, add spectator to game
    if message is KEY, call game key function, return game key function
    otherwise, send error
    return false

#### gameOver():

    call game_delete to free all memory
    print server shutting down message
    call message_done() to exit

---

## game module

### Function prototypes

```c
/* create and delete */

game_t* game_new(char* mapFileName);
void game_delete(game_t* game);
void end_game(game_t* game);

/* receive input */

void game_addPlayer(game_t* game, addr_t playerAddr, const char* message);
void game_addSpectator(game_t* game, addr_t newSpectator);

/* key press functions */

bool game_Q_quitGame(game_t* game, addr_t player, const char* message);

/* game_[KEY]_move[DIRECTION] */

bool game_h_moveLeft(game_t* game, addr_t player, const char* message);
bool game_l_moveRight(game_t* game, addr_t player, const char* message);
bool game_j_moveDown(game_t* game, addr_t player, const char* message);
bool game_k_moveUp(game_t* game, addr_t player, const char* message);
bool game_y_moveDiagUpLeft(game_t* game, addr_t player, const char* message);
bool game_u_moveDiagUpRight(game_t* game, addr_t player, const char* message);
bool game_b_moveDiagDownLeft(game_t* game, addr_t player, const char* message);
bool game_n_moveDiagDownRight(game_t* game, addr_t player, const char* message)

/* game_[CAPITALKEY]_move[DIRECTION] */

bool game_H_moveLeft(game_t* game, addr_t player, const char* message);
bool game_L_moveRight(game_t* game, addr_t player, const char* message);
bool game_J_moveDown(game_t* game, addr_t player, const char* message);
bool game_K_moveUp(game_t* game, addr_t player, const char* message);
bool game_Y_moveDiagUpLeft(game_t* game, addr_t player, const char* message);
bool game_U_moveDiagUpRight(game_t* game, addr_t player, const char* message);
bool game_B_moveDiagDownLeft(game_t* game, addr_t player, const char* message);
bool game_N_moveDiagDownRight(game_t* game, addr_t player, const char* message);
```

### Detailed pseudo code

#### game_new

    mallocs for a new game, return NULL if fail
    initializes a new roster
    initializes an empty spectator
    saves map from map file pathname to a full map, and an uneditable map
    saves map rows and columns
    initializes remaining gold count and number of players
    calls game_setGold

#### game_setGold

    creates a new empty gold map that is the same size as the game's map
    allocates a random number of gold piles, dropping them in random locations on the map
    each pile as at least one gold nugget

#### game_delete() 

    delete roster players
    delete all the maps
    free all game struct info

#### end_game

    create game over message
    send game over message to all players

#### game_addSpectator()

    if the client is a spectator
        make sure not a player 
        remove the old spectator
        set new spectator as spectator 
        send GOLD
        send GRID
        send DISPLAY

#### game_addPlayer() 

    check if at max player
        send quit
    check if spectator is sending a key press
        send error
    check if name is provided
        if only "" then send quit
    
    increment the number of players
    create a new player
        call functions from player module
        add to roster
        initialize the random location of player
            not on top of another player
        call functions from grid module
            set the grid, make visible to player
        update player visibility
    send "OK playerID" to client 
    send GRID GOLD DISPLAY to client
    update all the users of the game

#### game_keyPress()

    get player from address 
    send error and return flase if doesnt work
    switch of the message from client
        case Q
            call game_Q
        case h
            call game_h
        case j
            call game_j
        case k
            call game_k
        case l
            call game_l
        case y
            call game_y
        case u
            call game_u
        case n
            call game_n
    default: send message to player "ERROR"

#### game_Q_quitGame
    
    if spectator
        send quit message to spectator
    if player
        reset player's location to original map symbol
        EXTRA CREDIT: if player has gold,
            drops player's purse into a new gold pile at player's location
            send gold update to all users
    clear player's address in roster
    send quit message to player
    send display update to all players

#### game_foundGold

	gets number of nuggets in pile
	updates player gold
	if there is no more remaining gold
		call end_game and return true
	else
		reset original map place
		send new gold message to all users
		update spectator with gold message
		send display update to all users

#### game_stealGold

	if victim has nuggets
		decrement their purse
		increment player purse
		send message to both
	else
		send fail steal message to thief

#### lowercase key presses

	if is spectator, don't allow message
	otherwise, get player
	make sure next step is not out of bound
	if next step is a spot but not another player
		if next step is gold, call found gold and return true if game over
		reset player's original spot
		move player and update their visibility
		send display update to all users
	if next step is another player
		swap players
		call game_stealgold
		update both player's visibility
		send display update to all users

#### uppercase key presses

	if is spectator, don't allow message
	otherwise, while next spot is valid spot,
		call lowercase key press
		return if lowercase key press returns true
		increment player position

### Major data structures

```c
typedef struct game {
    roster_t* players;       // holds char* playerID to player_t* player
    int numbPlayers;
    addr_t spectator;
    grid_t* originalMap;
    grid_t* fullMap;
    grid_t* goldMap;
    gold_t* goldNuggets;
    int mapRows;
    int mapCols;
    int remainingGold;
} game_t;
```

---

## player module

### Functional decomposition

```c
player_t* player_new();
void player_delete(player_t* player);
void player_setAddress(player_t* player, addr_t address);
void player_setName(player_t* player, char* name);
void player_initializeGridAndLocation(player_t* player, grid_t* visibleGrid, grid_t* visibleGold, int locationX, int locationY);
void player_moveUpAndDown(player_t* player, int steps, char resetMapSpot);
void player_moveLeftAndRight(player_t* player, int steps, char resetMapSpot);
void player_foundGoldNuggets(player_t* player, int numGold);
void player_updateVisibility(player_t* player, grid_t* fullMap, grid_t* goldMap);

/* getters */
addr_t player_getAddr(player_t* player);
char player_getID(player_t* player);
char* player_getName(player_t* player);
int player_getXLocation(player_t* player);
int player_getYLocation(player_t* player);
grid_t* player_getMap(player_t* player);
grid_t* player_getVisibleGold(player_t* player);
int player_getGold(player_t* player);
```

### Pseudo code for logic/algorithmic flow

#### player_new()

    initializes player
    assigns player unique player ID based on global player ID char
    start player purse with 0

#### player_delete()

    free player name
    delete player maps
    free player

#### player_initializeGridAndLocation

    set player information to initialized location
    set player's viisble gold map

#### player_moveUpAndDown and player_moveLeftAndRight

    reset player's previous position in it's own visible map
    change x or y location
    set player location to @

#### player_updateVisibility

    adds additional visible map to player's existing visible map
    update player's visible gold map

### Major data structures

```c
typedef struct player {
    addr_t playerAddress;           // player address
    char playerID;                  // unique ID starting from A, B, C...
    char* playerName;               // player real name that client inputs
    int playerXLocation;            // player location x value
    int playerYLocation;            // player location y value
    int numGold;                    // player wallet
    grid_t* visibleMap;             // player's visible map
    grid_t* visibleGold;            // player's visible gold
} player_t;
```

---

## gold module

### Functional decomposition

```c
gold_t* gold_new(int totalPiles);
void gold_addGoldPile(gold_t* gold, int row, int col, int nuggets);
int gold_foundPile(gold_t* gold, int row, int col);
void gold_delete(gold_t* gold);
```

### Pseudo code for logic/algorithmic flow

#### gold_new()

```
    allocate memory for new gold data
    if memory allocation is successful
        set the total number of gold piles
        create a new set for gold piles
    return the new gold data
```

#### gold_addGoldPile()

```
    allocate memory for a new gold pile
    if memory allocation is successful
        set the row and column for the gold pile
        set the number of nuggets in the pile
        set the collected flag to false
        insert the new gold pile into the gold data set
        increment the gold ID
```

#### gold_foundPile_Helper()

```
    get the find gold pile data
    get the current gold pile
    if the current gold pile's row and column match the find pile's row and column
        set the collected flag to true
        set the matched pile in the find gold pile data to the current pile
```

#### gold_foundPile()

```
allocate memory for find gold pile data
if memory allocation is successful
        set the row and column to be found
        set the matched pile to null
        iterate over the gold data set with the helper function
        get the matched pile from the find gold pile data
        free the find gold pile data
 if no matched pile is found, return -1
 return the number of nuggets in the matched pile
```

#### gold_delete()
```
  free the gold data
```

### Major data structures

```c
typedef struct goldPile {
    int goldRow;
    int goldCol;
    int numNuggets;
    int collected;
} goldPile_t;

typedef struct gold {
    int numPiles;
    set_t* piles;
} gold_t;

typedef struct findGoldPile {
    int findRow;
    int findCol;
    goldPile_t* matchedPile;
} findGoldPile_t;
```

---

## roster module

### Functional decomposition

```c
roster_t* roster_new();
bool roster_addPlayer(roster_t* roster, player_t* player);
void roster_updateAllPlayers(roster_t* roster, game_t* fullMap);
void roster_updateAllPlayersGold(roster_t* roster, game_t* fullMap);
char* roster_createGameMessage(roster_t* roster);
void roster_delete(roster_t* roster);
player_t* roster_getPlayerFromAddr(roster_t* roster, addr_t playerAddr);
player_t* roster_getPlayerFromID(roster_t* roster, char playerID);
```

### Pseudo code for logic/algorithmic flow

#### roster_new()
```
    allocate memory for new roster
    if memory allocation is successful
        create a new set for players
    return the new roster
```

#### roster_addPlayer()
```
    get the player ID
    insert the new player into the roster set
    return the result of the insertion operation
```

#### roster_updateAllPlayers()
```
iterate over the players set with the update helper function
```

#### roster_updateAllPlayersGold()
```
iterate over the players set with the gold update helper function
```

#### roster_createGameMessage()
```
    allocate memory for game over message
    format the game over message
    iterate over the players set 
    iterate over the players set again to send the game over message
    return the game over message
```

#### roster_delete() 
```
    delete the players set with the delete helper function
    free the roster
```

#### roster_getPlayerFromAddr()
```
    allocate memory for find player pack
    if memory allocation is successful
        set the address to be found
        set the found player to null
        iterate over the players set with the address helper function
        get the found player from the find player pack
        free the find player pack
    return the found player
```

#### roster_getPlayerFromID()
```
    allocate memory for find player pack
    if memory allocation is successful
        set the ID to be found
        set the found player to null
        iterate over the players set with the ID helper function
        get the found player from the find player pack
        free the find player pack
    return the found player
```

### Major data structures

```c
typedef struct roster {
    set_t* players;         // key: playerID    value: player_t*
} roster_t;

typedef struct findPlayerPack {
    addr_t matchAddress;
    char matchPlayerID;
    player_t* foundPlayer;
} findPlayerPack_t;
```

---

## Testing plan

### unit testing

Using miniserver and miniclient, we were able to make small versions of grid, game, and player to test that the modules were working correctly. Roster and gold were added afterwards so they were tested with server and client running.

### integration testing

Using miniserver and miniclient, we were able to test server and client individually. For example, we would send server messages directly from the commandline using miniclient with "PLAY" or "KEY" to test server functionality without client, and similarly we would send client messages directly from the commandline using miniserver with "DISPLAY" or "GOLD" messages.

### system testing

We ran server and client together on different servers and different machines to ensure functionality. We tested ./server and ./client on all the maps in the maps directory (not including contributions directories), and manually opened up to 26 client windows to add to the game.

---

## Limitations

**No resetting player ID's in `server`:**
If a player quits, their player ID is never reused, meaning that the 27th player who joins after a previous player quits normally will get a non-alpha character. Then the switch conflicting player functionality will not work if another player tries to switch with the player with non-alpha ID.