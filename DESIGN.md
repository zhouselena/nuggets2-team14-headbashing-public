# CS50 Nuggets
## Design Spec
### Team 14 - Headbashing, Spring, 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes map modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Player

> Teams of 3 students should delete this section.

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI.

> You may not need much more.

### Inputs and outputs

> Briefly describe the inputs (keystrokes) and outputs (display).
> If you write to log files, or log to stderr, describe that here.
> Command-line arguments are not 'input'.

Input:
Output:


### Functional decomposition into modules

> List and briefly describe any modules that comprise your client, other than the main module.
 
### Pseudo code for logic/algorithmic flow

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> See the Server section for an example.

> Then briefly describe each of the major functions, perhaps with level-4 #### headers.

acceptConnection()

### Major data structures

> A language-independent description of the major data structure(s) in this program.
> Mention, but do not describe, any libcs50 data structures you plan to use.

---

## Server
### User interface

The severs's only interface with the user is on the command-line; it requires one argument and can optionally take a second:

``` 
./server map.txt [seed]
```

The server has no furhter interaction with the user after it has been launched. The command-line interface takes the pathname for a map file as its first argument, with an optional second argument serving as a seed for the random-number generator. If a seed is provided, it must be a positive integer.

### Inputs and outputs

Input:
- Map file: A text file that contains a map layout of the game. This is read from the command-line and parsed into a 2D array. 
- Server also recieve messages from client: `PLAY`, `SPECTATE`, `KEY`

Output:
- Map representation: The server sends a string representation of the map file to the client.
- Terminal output: Upon launch, the server outputs the port number for client connections After all gold nuggets are collected, it prepares a summary, sends a QUIT message to all clients, prints the summary to the terminal, and exits.

### Functional decomposition into modules

> List and briefly describe any modules that comprise your server, other than the main module.

- Map Parse: Module responsible for parsing the map. The server is required to load a map from the file provided in order to know the layout of the game world. The map file is stored in text format and needs to be read and converted into a data structure that the program was use. 
- Connection Manager: Module responsible for incoming client connections. It handles the networking including accepting connections, maintnig a list of currenly connected clients, and creates the seperate threads that handles the each client. 
- Game Engine: Module responsible for processing inputs from the clients (such as player moves), updates the game state accordingly (moving players, collecting gold nuggets, etc.), and then sends the updated game state back to the clients. The game engine would use the data structure created by the Map Parser to know the layout of the game world.


### Pseudo code for logic/algorithmic flow

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

The server will run as follows:

	parses command line for number of arguments
	check if can open map, add randomized gold nuggets
	start connection and wait for clients to connect
	while game is not over:
		if/for each client that connects:
			if spectator, boot current spectator if exists
				enter spectator mode
			else
				add player
				draw map
				for all players that exist, add to map
				check for player input
					if quit, then quit
					if move, then update their location and information
					if there is no more gold to get, then end game

	execute from a command line per the requirement spec
	parse the command line, validate parameters
	call initializeGame() to set up data structures
	initialize the 'message' module
	print the port number on which we wait
	call message_loop(), to await clients
	call gameOver() to inform all clients the game has ended
	clean up

We anticipate the following major functions:

parseArgs()
initializeGame()
message_loop()
initializeConnection()
drawMap()
gameOver()

### Major data structures

> Describe each major data structure in this program: what information does it represent, how does it represent the data, and what are its members.
> This description should be independent of the programming language.
> Mention, but do not describe, data structures implemented by other modules (such as the new modules you detail below, or any libcs50 data structures you plan to use).

- Map module that is described below.
- Player struct: Holds information for each player, including port ID, player ID, player name, location, gold count, vision grid, and flags for whether the player has seen all spots on the map.
- Counter of gold piles in the map and its locations
- Game struct: Contains all game-related data including an array of players, game map, and counters for gold piles.

---

### Major data structures

> Describe each major data structure in this module: what information does it represent, how does it represent the data, and what are its members.
> This description should be independent of the programming language.

Map: This data structure represents the game map. It contains information about the layout of the map and the locations of gold nuggets. It will be represented as a 2D array.

---

## Player Struct module

> Repeat this section for each module that is included in either the client or server.

### Functional decomposition

> List each of the main functions implemented by this module, with a phrase or sentence description of each.

initializeConnection(): Initializes networking components and prepares the server to accept incoming connections.
acceptConnection(): Listens for and accepts incoming client connections.
manageConnection(): Maintains the list of active clients and handles the creation of separate threads for each client.


### Pseudo code for logic/algorithmic flow

> For any non-trivial function, add a level-4 #### header and provide tab-indented pseudocode.
> This pseudocode should be independent of the programming language.

initializeConnection()::

	set up server socket 
	bind server to specified port
	set server to listen state
	returns server socket

acceptConnection():

	receive 

manageConnection():

	receive 

### Major data structures

> Describe each major data structure in this module: what information does it represent, how does it represent the data, and what are its members.
> This description should be independent of the programming language.

Active client list: A list of currently connected clients, containing client socket descriptors and related client information.