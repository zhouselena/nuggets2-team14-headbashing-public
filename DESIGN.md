# CS50 Nuggets
## Design Spec
### Team 14 - Headbashing, Spring, 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes map modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

As we are a team of two, we were provided grid so this will not be covered in our design spec. 

## Player/Client
The client acts in one of two modes:

1. spectator, the passive spectator mode described in the requirements spec.
2. player, the interactive game-playing mode described in the requirements spec.

### User interface
The client's interface with the user is on the command-line; it requires two arguments and optionally takes a third:
```bash
 ./client hostname port [playername]
```
The user also interacts with the game through stdin that allows the player to move about and quit the game. 

### Inputs and outputs
Input: 
- The client receives keystrokes from the user as input. These keystrokes are interpreted as player commands and sent to the server.
    - `Q` quit the game.
    - `h` move left, if possible
    - `l` move right, if possible
    - `j` move down, if possible
    - `k` move up , if possible
    - `y` move diagonally up and left, if possible
    - `u` move diagonally up and right, if possible
    - `b` move diagonally down and left, if possible
    - `n` move diagonally down and right, if possible
    - where possible means the adjacent gridpoint in the given direction is an empty spot, a pile of gold, or another player.
    - for each move key, the corresponding Capitalized character will move automatically and repeatedly in that direction, until it is no longer possible.
- The client receives messages from the server, such as `OK`,  `GRID`, `GOLD`, `DISPLAY`, `QUIT`, `ERROR` messages. These messages update the state of the game and the display for the player.
- The client receives a string map from the server

Output:
- The client sends the keystrokes to the server as a player's move or actions. At anytime it will send,  `KEY k`, where the k is the single-character keystroke typed by user
- The client displays the game grid, status, and any received messages on the user's screen.

### Functional decomposition into modules
- Grid module: Module responsible for holding information about the map. The map file will be loaded into a grid module and the grid will be used when traversing over the map.

### Pseudo code for logic/algorithmic flow
The client will run as follows:
    ```
    parses command line for number of arguments
    validate arguments 
    if arguments are valid:
        hostname = get hostname from arguments
        port = get port from arguments
        playername = get playername from arguments (if exists)
                initialize the game structure
        initialize the display
        initialize the network

        if the playername exists:
            send message to the server: "PLAY " + playername
        else:
            send message to the server: "SPECTATE"

        while game is not over:
            message = receive message from server
            process the server message
            update the display

            if EOF on stdin:
                send message to the server: "KEY Q"

        logMessage("Game-over")
        exit with zero
    else:
        print "Invalid command line arguments"
        exit with non zero
    ```
We anticipate the following major functions:
#### main()
Initializes client structure, parses command-line arguments, initializes display, sets up network connection, enters game loop, and performs cleanup when the game is over.

    ```
        Allocate memory for client structure
        parse args
        initialize display and network
        enter the game loop
            process server messages or handle the user inputs
        delete the window
        free allocated memory
        end game
    ```

#### parseArgs(): 
Checks the validity of command-line arguments and initializes the client structure accordingly.

    ```c
    check the validity of command-line arguments
    initialize the clientStruct accordingly
    ```

#### initializeDisplay():
Sets up the game display.

    ```c
        Start ncurses mode and create window
        Set keyboard mapping and dont display the key press 
        Create window for displaying the game
        Refresh the screen to display changes
    ```

#### initializeNetwork():
Sets up the network connection, sends the initial message to the server, and starts the message loop.

	```c
		For the client to server message
		if player name is NULL or empty, client is a spectator. Otherwise, player
		Convert port to string
		set up server details using hostname and port
		Initialize the message server
		Join the server
		Start message loop
	```

#### handleMessage():
Handles messages received from the server, updates the game state, and refreshes the display.

	```c
	Handle OK message
	Handle GRID message
	Handle GOLD message
	Handle DISPLAY message
	Handle QUIT message
	Handle ERROR message
	Handle Unknown/ Misordered message
	```

#### handleInput():
Handles user key presses and sends corresponding messages to the server.

	```c
	read the key pressed by the user
	send a corresponding message to the server
	```

### Major data structures

1. ClientStruct: This structure represents a client, holding information if the client is a player or spectator, player's name, player ID, gold count, total gold, location in the grid, window, server address.

---

## Server
### User interface

The severs's only interface with the user is on the command-line; it requires one argument and can optionally take a second:
```bash
./server map.txt [seed]
```
The server has no further interaction with the user after it has been launched. The command-line interface takes the pathname for a map file as its first argument, with an optional second argument serving as a seed for the random-number generator. If a seed is provided, it must be a positive integer.

### Inputs and outputs
Input:
- Map file: A text file that contains a map layout of the game provided by the command-line argument. The server parses this file to initialize the game grid and setup random gold piles. Optionally, a random seed can also be provided as a command-line argument.
- Server also receives these messages from client: `PLAY`, `SPECTATE`, `KEY`

Output:
- Map representation: The server sends a string representation of the map file to the client.
- Terminal output: Upon launch, the server outputs the port number for client connections. After all gold nuggets are collected, it prepares a summary, sends a `QUIT`, message to all clients, prints the summary to the terminal, and exits.

### Functional decomposition into modules
- Game module: Module responsible for processing inputs from the clients (such as player moves), updates the game state accordingly (moving players, collecting gold nuggets, etc.), and then sends the updated game state back to the clients. The game engine would use the data structure created by the Map Parser to know the layout of the game world.
- Player module: Module responsible for holding information for each individual client (such as player position, number of gold nuggets, player name/ID) that will be updated every time the client sends a command.
- Roster module: Structure that is reponsible for holding the infromation of the roster of players that are in the game
- Gold module: Structure responsible for holding the data about the gold, including the incrementing counts and placement if the gold
- Grid module: Module responsible for holding information about the map. The map file will be loaded into a grid module and the grid will be used when traversing over the map.

### Pseudo code for logic/algorithmic flow
The server will run as follows:
```
parses to validate command line for number of arguments
	initializeGame using the map file
	initialize the message module and print the port number
	call message_loop to await messages from clients:
		for each client that connects:
			  handle the message that is sent
		upon receipt of EOF or a QUIT command, call game_over to shut down the server
```
We anticipate the following major functions:

#### parseArgs():
Parses and validates the command-line arguments. It exits with a nonzero status if the arguments are incorrect or invalid.
```
If the number of arguments is less than 2 or more than 3 
    Print usage message, exit
Open the map file from the argument list
    If fail, print error, exit
Close the map file
If there is a third argument 
    Parse the seed from the argument
    If fail, print error, exit
    Set the random seed to the parsed seed
Else
    Set the random seed to the process id
```

#### initializeGame():
This function initializes the game locally, creating the game/grid from the provided map file and setting up random gold piles.
```
make new game from the provided map file name
if game creation fails then,
    error and exit
```

#### handleInput():
This function handles input from stdin and is used to detect EOF.
```
If EOF is reached, true
else, return false
```

#### handleMessage():
This function is responsible for handling messages from clients. It calls appropriate game functions based on the input message from a client.
```
If the message is "PLAY" 
    Add a new player to the game 
If the message starts with "SPECTATE" 
    Add a new spectator to the game 
If the message starts with "KEY" 
    Handle a key press 
    Return the result of the key press handler
If else, error
Return false
```

#### gameOver():
This function frees all resources related to the game and shuts down the server gracefully by calling `message_done()`.
```
Delete the game
Print a shutdown message
Call the message_done function
```

### Major data structures
- Player struct: Holds information for each player, including port ID, player ID, player name, location, gold count, vision grid, and flags for whether the player has seen all spots on the map.
- Game struct: Contains all game-related data including an array of players, game map, and counters for gold piles.
- Grid: This data structure represents the game map. It contains information about the layout of the map and the locations of gold nuggets.
- libcs50 modules: counter of gold piles in map and its locations, hashtable of players

---

## Player module
Player is included in both client and server.

### Functional decomposition
We anticipate the following major functions:

- player_new() — Allocates memory for a new player. Sets the player's unique ID and initializes gold to zero.
- player_delete() — Frees all allocated memory for the player, including player name, visible map, visible gold, and the player itself.
- player_setAddress() — Sets the player's address.
- player_setName() — Sets the player's name.
- player_initializeGridAndLocation() — Sets the player's visible map, location, and initializes the player's visible gold map based on the - - provided visible grid and gold map.
- player_moveUpAndDown() — Moves the player up or down the grid by a certain number of steps and updates the player's visible map.
- player_moveLeftAndRight() — Moves the player left or right on the grid by a certain number of steps and updates the player's visible map.
- player_foundGoldNuggets() — Increases the player's gold count by a certain amount.
- player_updateVisibility() — Updates the player's visibility based on the full map and gold map.
- player_serverMapUpdate() — Updates the player's visible map based on the server's game map and gold map.

Getters
- player_getAddr() — Returns the player's address.
- player_getID() — Returns the player's ID.
- player_getName() — Returns the player's name.
- player_getXLocation() — Returns the player's x coordinate on the map.
- player_getYLocation() — Returns the player's y coordinate on the map.
- player_getMap() — Returns the player's visible map.
- player_getVisibleGold() — Returns the player's visible gold map.
- player_getGold() — Returns the amount of gold the player has collected.

### Pseudo code for logic/algorithmic flow
#### player_initializeGridAndLocation() 
#### player_moveUpAndDown()
#### player_moveLeftAndRight() 
#### player_updateVisibility() 
#### player_serverMapUpdate() 

### Major Data Structures
Player Struct
- playerAddress: Address of the player.
- playerID: Unique ID for the player, incrementing from 'A'.
- playerName: Name of the player given by the client input.
- playerXLocation & playerYLocation: The current location coordinates of the player on the grid.
- numGold: The amount of gold the player has collected.
- visibleMap: The grid of the map that is currently visible to the player.
- visibleGold: The grid that shows the gold which is currently visible to the player.

---

## Game module
### Functional decomposition
Active client list: A list of currently connected clients, containing client socket descriptors and related client information.
- game_new() — Initializes a new map and info, sets up gold. 
- game_delete() — Funtion frees the informaiton of game and deletes it
- end_game() — Sends a GAME OVER message when all gold is picked up
- game_addPlayer() — Adds a player to the game if it is not full
- game_addSpectator() - Funtion called when server handled the mesage SPECTATE from the client. Adds spectator and kicks other one out
- game_keyPress() - function called to handle keys pressed from the player --- calls on each letter press helper function
- game_returnFullMap() - Function that returns a map with all the players ID
- game_returnGoldMap() - Function that returns a map with the gold locations that are all uncollected
- game_returnRemainingGold() - Functions that says how many gold piles are left
- helper messages that send the GRID, OK, DISPLAY, GOLD, ERROR, QUIT, messages to client

### Pseudo code for logic/algorithmic flow
#### game_new()
```
    create a new roster if player logs on
    initialize all the variables of game struct
        set spectator to message_noAddr
        set maps
        set the grid rows and columns
        set the remaining gold to the total
        set the number of players
    return the game information

```
#### game_delete() 
```
    delete roster players
    delete all the maps
    free all game struct info
```
#### end_game()
```
    call roster functions to make the send message
    send to player
    send to spectator
```
#### game_addSpectator()
```
    if the client is a spectator
        make sure not a player 
        remove the old spectator
        set new spectator as spectator 
        send GOLD
        send GRID
        send DISPLAY
```
#### game_addPlayer() 
```
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
         

```
#### game_keyPress()
```
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

```


### Major Data Structure
Game struct:
- Roster of players
- Number of players
- Original map
- Full map
- Gold map
- Gold map
- Map rows
- Map column
- Number of remaining gold

---

## Gold Structure — Module
The Gold module manages gold piles in the game. It contains the following key structures:
- `goldPile_t`: This represents a pile of gold in the game. It contains the following data:
    - `goldRow` & `goldCol`: The coordinates (row and column) of the gold pile on the game grid.
    - `numNuggets`: The number of gold nuggets in the pile.
    - `collected`: A flag indicating whether the gold pile has been collected or not.
- `gold_t`: This represents the overall gold data in the game. It contains the following data:
    - `numPiles`: The total number of gold piles.
    - `piles`: A set data structure containing all the gold piles.

### Functional decomposition
- gold_new()— create new data for new gold 
- gold_addGoldPile()— add a new gold pile including setting the row, number of nuggets, and incrementing gold 
- gold_foundPile_Helper()— helper function called when player finds a fil
- gold_foundPile()— finding a gold pile. returns number of nuggets in pile
- gold_delete()— detes the gold data set and frees it

### Pseudo Code for Logic/Algorithmic Flow
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

#### gold_foundPile()`
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
#### gold_delete()`
```
  free the gold data
```

---

## Roster Structure — Module
The Roster module manages a set of players in the game. 

### Functional decomposition
- roster_new()— create new roster
- roster_addPlayer()— add new player to roster
- roster_updateAllPlayers()— update all players in roster
- roster_updateAllPlayersGold()— update gold for all the players in the roster 
- roster_createGameMessage()— create the game over message
- roster_delete()— delete the roster 
- roster_getPlayerFromAddr()— gets the player from the roster by address
- roster_getPlayerFromID()— gets the player from the roster by its ID

### Pseudo Code for Logic/Algorithmic Flow
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

### Major Data Structure
It contains the following key structures:
- `roster_t`: This represents a roster of players in the game. It contains a set of players.
- `findPlayerPack_t`: This structure is used when searching for a specific player in the roster. It contains the following data:
    - `matchAddress`: The address of the player to be found.
    - `matchPlayerID`: The player ID of the player to be found.
    - `foundPlayer`: The found player, if any.