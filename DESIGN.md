# CS50 Nuggets
## Design Spec
### Team name, term, year

> This **template** includes some gray text meant to explain how to use the template; delete all of them in your document!

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
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

See the requirements spec for the command-line interface.
There is no interaction with the user.

> You may not need much more.

### Inputs and outputs

> Briefly describe the inputs (map file) and outputs (to terminal).
> If you write to log files, or log to stderr, describe that here.
> Command-line arguments are not 'input'.

### Functional decomposition into modules

> List and briefly describe any modules that comprise your server, other than the main module.

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

---

## XYZ module

> Repeat this section for each module that is included in either the client or server.

### Functional decomposition

> List each of the main functions implemented by this module, with a phrase or sentence description of each.

### Pseudo code for logic/algorithmic flow

> For any non-trivial function, add a level-4 #### header and provide tab-indented pseudocode.
> This pseudocode should be independent of the programming language.

### Major data structures

> Describe each major data structure in this module: what information does it represent, how does it represent the data, and what are its members.
> This description should be independent of the programming language.
