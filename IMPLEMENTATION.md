# CS50 Nuggets
## Implementation Spec
### Team 14- Headbashing, Spring 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes `game`, `gold`, `roster`, `player`, `grid` modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

Kyla: `client`, `player`, `DESIGN.md`, scrum
Selena: `server`,`game`,`roster`,`gold`
Both: testing, IMPLEMENTATION.md, README.md, MAKEFILES, .gitignore

## Player
### Data structures
Client module utilizes one main structure that is the clientStruct.
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
Initializes client structure, parses command-line arguments, initializes display, sets up network connection, enters game loop, and performs cleanup when the game is over.
```c
int main(const int argc, char* argv[]);
```
A function to parse the command-line arguments and validate them. Sets isPlayer to true or false
```c
static void parseArgs(const int argc, char* argv[]);
```
A function to set up the game display
```c
void initializeDisplay();
```
Set up of the network connection, sends the initial message to the server, and starts the message loop.
```c
void initializeNetwork(char* serverHost, char* port, FILE* errorFile, char* playerName);
```
Handles messages received from the server, updates the game state, and refreshes the display.
```c
static bool handleMessage(void* arg, const addr_t incoming, const char* message);
```
Handles user key presses and sends corresponding messages to the server.
```c
static bool handleInput(void* arg);
```

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
		dispaly message
	Handle Unknown/ Misordered message

#### `handleInput`:

	read the key pressed by the user 
	if input recognized
		send a corresponding message to the server
	continue processing

---

## Server

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	verify map file can be opened for reading
	if seed provided
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()

---

## XYZ module

> For each module, repeat the same framework above.

### Data structures

### Definition of function prototypes

### Detailed pseudo code

---

## Testing plan

### unit testing

> How will you test each unit (module) before integrating them with a main program (client or server)?

### integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

### system testing

> For teams of 4: How will you test your client and server together?

---

## Limitations

> Bulleted list of any limitations of your implementation.
> This section may not be relevant when you first write your Implementation Plan, but could be relevant after completing the implementation.
