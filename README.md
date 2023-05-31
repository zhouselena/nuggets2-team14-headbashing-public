# Nuggets

### Description
This repository contains the code for the CS50 "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets.
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms.
Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

### Usage
The global `make all` creates the executables `server` and `client`, and directories `common` and `support` required by the executables. Specific information can be found in each directories respective `README.md`'s.

To run server, run `./server [mapFilePath] [optional seed]`. Upon proper execution, it will print out a port number that `client` must refer to.

To run client, server must be running first. Run `./client [hostname] [portnumber] [optional player name to play, or empty to spectate] 2>player.log`.
Both player and spectator may send `Q` at any time to stop participating.
The player may use the following keystrokes to move:
* `h`: move left
* `l`: move right
* `k`: move up
* `j`: move down
* `y`: move diagonally up and left
* `u`: move diagonally up and right
* `b`: move diagonally down and left
* `n`: move diagonally down and right
Holding shift while sending any keystroke will allow the player to "run".

### Directories:
* `common`: all shared modules
* `support`: provided modules (`message.h`)

## Known Issues

**Memory leaks in `server`**
Server frees most of all malloc'd memory, but has memory leaks anywhere set_iterate is called on a pack data type. Namely, in `roster_getPlayerFromAddr` and `roster_getPlayerFromID`, the data type `findPlayerPack` is not free'd before returning the player. Server also overwrites existing memory in `roster_createGameMessage_Helper` when the "GAME OVER" message is being constructed.