# Nuggets

### Description
This repository contains the code for the CS50 "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets.
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms.
Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

For more detailed information on design and implementation, please refer to `DESIGN.md` and `IMPLEMENTATION.md`. For our team's scrum, please check this repository's wiki.

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
* `maps`: provided map files, and the map file we created (`headbashing.txt`)

### Extra Credit
1. If a player quits the game, any gold they had collected will be released back to the game in a pile at their last position.
2. Players may steal gold nuggets from each other. If player 'X' steps on player 'Y', player 'Y' will lose 1 gold nugget and receive a message, and player 'X' will gain 1 gold nugget and receive a message. If player 'Y' didn't have any gold in their purse, player 'X' will receive a message to let them know they couldn't steal from player 'Y'. Finally, player 'X' and player 'Y' will switch places as usual.

### Known Issues

**No resetting player ID's in `server`:**
If a player quits, their player ID is never reused, meaning that the 27th player who joins after a previous player quits normally will get a non-alpha character. Then the switch conflicting player functionality will not work if another player tries to switch with the player with non-alpha ID.
