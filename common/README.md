# 'common' directory for Nuggets

### Selena Zhou, Kyla Widodo, 23S

> This directory is required for `make all` in the overall directory.

To make, run `make` in terminal.

The 'common' directory holds all the important modules that both client and server use.

### New modules included:
* `game.h`: holds all maps, players, spectator, and game functionalities required by server.
* `player.h`: player data type for each client who is a player
* `gold.h`: holds information about gold piles in map
* `grid.h`: data type to hold information about maps
* `roster.h`: holds a set of players for `game`

### Previously created modules:
* `mem.h`: used in client
* `set.h`: used in game, roster, gold
