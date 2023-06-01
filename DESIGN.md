# CS50 Nuggets
## Design Spec
### Team 14 - Headbashing, Spring 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes `game`, `player`, `roster`, and `gold` modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

As we are a team of 2, we were provided with `grid` so this will not be covered in our design spec. 

## Player/Client

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

The client's interface with the user is on the command-line; it requires two arguments and optionally takes a third:

```bash
 ./client hostname port [playername]
```

The user also interacts with the game through stdin that allows the player to move about and quit the game.

### Inputs and outputs

**Inputs:**

The client receives keystrokes from the user as input. These keystrokes are interpreted as player commands and sent to the server.
- `Q` quit the game.
- `h` move left, if possible
- `l` move right, if possible
- `j` move down, if possible
- `k` move up, if possible
- `y` move diagonally up and left, if possible
- `u` move diagonally up and right, if possible
- `b` move diagonally down and left, if possible
- `n` move diagonally down and right, if possible
- where possible means the adjacent gridpoint in the given direction is an empty spot, a pile of gold, or another player.
- for each move key, the corresponding Capitalized character will move automatically and repeatedly in that direction, until it is no longer possible.

The client receives messages from the server, including `OK`, `GRID`, `GOLD`, `GOLDSTEAL`, `DISPLAY`, `QUIT`, `ERROR` messages. These messages indicate an update in the state of the game and call for an update to the display for the player.

**Output:**

- The client sends the keystrokes to the server as a player's move or actions. At anytime it will send, `KEY k`, where the k is the single-character keystroke typed by the user.
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

#### parseArgs(): 
Checks the validity of command-line arguments and initializes the client structure accordingly.

#### initializeDisplay():
Sets up the game display.

#### initializeNetwork():
Sets up the network connection, sends the initial message to the server, and starts the message loop.

#### handleMessage():
Handles messages received from the server, updates the game state, and refreshes the display.

#### handleInput():
Handles user key presses and sends corresponding messages to the server.

### Major data structures

1. `clientStruct`: This structure represents a client, holding information if the client is a player or spectator, player's name, player ID, gold count, total gold, location in the grid, window, server address. This data structure uses the `message.h` module.

---

## Server
### User interface

The server's only interface with the user is on the command-line; it requires one argument and can optionally take a second:

```bash
./server map.txt [seed]
```

The server has no further interaction with the user after it has been launched. The command-line takes a path that leads to the map file, and an optional seed. If the seed is provided it must be a positive integer.

### Inputs and outputs

**Inputs:**

The server receives no input from the user. The server receives the following messages from `client`:
- `PLAY playername`: tells server to add *playername* to the game if possible.
- `SPECTATE`: tells server to add a spectator.
- `KEY key`: tells server what keystroke was sent by a user.

**Outputs:**

The server sends the following messages to `client`:
- `OK playerID`: when a new player is added to the game, server immediately sends this.
- `GRID nrows ncols`: when a new user is added to the game, server immediately sends this.
- `GOLD n p r`: server sends this when there is an update with gold.
- `GOLDSTEAL n p r otherPlayerID`: server sends this when a player attempts to steal gold from another player.
- `QUIT message`: server sends this when a player needs to leave the game.
- `ERROR explanation`: server sends this when an unexpected error happens.

The server outputs two messages to terminal. Upon launch, it will print `Server is starting at port [portID]`. After the game ends, it will print `Server is shutting down`.

### Functional decomposition into modules

- `game`: reponsible for holding information for the game, generating the game, adding players, and processing player interactions.
- `player`: responsible for holding information for one player, including location, purse, and visible maps.
- `roster`: responsible for holding a set of players and updating information for all players in the set.
- `gold`: resposible for holding information about each gold pile in the map.
- used provided modules: `grid`, `message`

### Pseudo code for logic/algorithmic flow

The server will run as follows:

```
parses to validate command line for valid arguments
initialize game using the map file
initialize the message module and print the port number
call message_loop to await messages from clients:
    for each client that connects:
            handle the message that is sent
    upon receipt of EOF or a QUIT command, call game_over to shut down the server
```
We anticipate the following major functions:

#### parseArgs():
Parses and validates the command-line arguments. It exits with a nonzero status if the arguments are incorrect or invalid.

#### initializeGame():
This function initializes the game locally, creating the game/grid from the provided map file and setting up random gold piles.

#### handleInput():
This function handles input from stdin and is used to detect EOF.

#### handleMessage():
This function is responsible for handling messages from clients. It calls appropriate game functions based on the input message from a client.

#### gameOver():
This function frees all information the game holds and shuts down the server gracefully by calling `message_done()`.

### Major data structures

- `game`: holds information about the entire game include a roster of players, all maps, and gold information.
- `player`: holds information for an individual player, including their address, player ID, player name, location, purse, and visible maps.
- `roster`: holds a set of players and info
- `gold`: holds a set of all gold piles and info about each pile
- other modules: `grid`, `set`, `message`

---

## Extra credit
* When a player exits the game with keystroke Q, their purse is dropped in a gold pile at the last location they were. Implemented in `game` module
* When a player steps on another player, they "steal" one gold nugget if available from that player. Implemented in `game` module

---

## game module

`game` is used in server.

### Functional decomposition

We anticipate the following major functions:
- game_new: allocates memory for new game and initializes sets and maps
- game_delete: frees all memory malloc'd in game struct
- end_game: sends GAME OVER summary to all clients
- game_addPlayer: if possible, add player to game
- game_addSpectator: add spectator game and kick out the previous spectator
- game_keyPress: call respective key functions
- game_Q_quitGame: let a player quit the game

We anticipate the following helper functions:
- game_setGold: sets up random number of gold piles and nuggets per pile all across the map
- game_sendOKMessage: constructs OK message to send to client
- game_sendGridMessage: constructs GRID message to send to client
- game_sendGoldMessage: constructs GOLD message to send to client
- game_sendDisplayMessage: constructs DISPLAY message to send to client
- game_foundGold: once player finds gold, updates map, gold count, and all clients
- game_stealGold: switches gold from one player's purse to another
- game_updateAllUsers: send DISPLAY update to all clients
- game_updateAllUsersGold: send GOLD update to all clients

We anticipate the following key functions:
- lowercase key presses: if possible, move one step towards specified direction. calls helper functions if next spot isn't a room spot
    - game_h_moveLeft
    - game_l_moveRight
    - game_j_moveDown
    - game_k_moveUp
    - game_y_moveDiagUpLeft
    - game_u_moveDiagUpRight
    - game_b_moveDiagDownLeft
    - game_n_moveDiagDownRight
- uppercase key presses: move as many steps towards specified direction as possible. in a loop calls the functino of its lowercase counterpart until it can't go any further
    - game_H_moveLeft
    - game_L_moveRight
    - game_J_moveDown
    - game_K_moveUp
    - game_Y_moveDiagUpLeft
    - game_U_moveDiagUpRight
    - game_B_moveDiagDownLeft
    - game_N_moveDiagDownRight

Getters:
- game_returnFullMap: returns map with rooms and player IDs
- game_returnGoldMap: returns map with all gold piles
- game_returnRemainingGold: returns how much gold is remaining

### Pseudo code for logic/algorithmic flow

Detailed pseudo code for each function can be found in `IMPLEMENTATION.md`.

### Major data structures

`game_t`
- roster of players
- number of players
- original map
- full map (with player ID's)
- gold map
- map rows
- map columns
- remaining gold count

---

## player module

`player` is used in server.

### Functional decomposition

We anticipate the following major functions:
- player_new: mallocs space for a new player, initializes its purse, and gives it a unique player ID
- player_delete: frees all space malloc'd for player
- player_initializeGridAndLocation: sets player's starting location, visible map, and visible gold map

updaters:
- player_moveUpAndDown: updates player's Y location
- player_moveLeftAndRight: updates player's X location
- player_foundGoldNuggets: updates player's purse
- player_updateVisibility: updates player's visible map

setters:
- player_setAddress
- player_setName

getters:
- player_getAddr
- player_getID
- player_getName
- player_getXLocation
- player_getYLocation
- player_getMap
- player_getVisibleGold
- player_getGold

### Pseudo code for logic/algorithmic flow

Detailed pseudo code for each function can be found in `IMPLEMENTATION.md`.

### Major data structures

`player_t`
- player address
- player ID
- player name
- player x, y coords
- player purse
- player visible map
- player visible gold map

---

## roster module

`roster` is used in server.

### Functional decomposition

We anticipate the following functions:
- roster_new: mallocs for roster and initializes player set
- roster_addPlayer: adds a new player to the set with key player ID
- roster_updateAllPlayers: sends DISPLAY message to all players in player set
- roster_updateAllPlayersGold: sends GOLD message to all players in player set
- roster_createGameMessage: creates GAME OVER message with all players and their purses
- roster_delete: frees all memory used by roster
- roster_getPlayerFromAddr: returns a player that matches a given address
- roster_getPlayerFromID: returns a player that matches a given ID

### Pseudo code for logic/algorithmic flow

Detailed pseudo code for each function can be found in `IMPLEMENTATION.md`.

### Major data structures

`roster_t`
- set of players

`findPlayerPack_t`
- address of player to be found
- ID of player to be found
- the found player

---

## gold module
`gold` is used in server.

### Functional decomposition

We anticipate the following functions:
- gold_new: mallocs space for gold and initializes gold pile set
- gold_addGoldPile: adds a gold pile to the set
- gold_foundPile: returns the number of nuggets at the given XY locatino
- gold_delete: frees all memory used by gold

### Pseudo code for logic/algorithmic flow

Detailed pseudo code for each function can be found in `IMPLEMENTATION.md`.

### Major data structures

`goldPile_t`
- x, y coordinates
- number of nuggets
- has it been collected yet?

`gold_t`
- set of gold piles
- number of gold piles

`findGoldPile_t`
- x, y coordinates of gold pile
- the gold pile at the x, y coords