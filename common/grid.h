/*
 * grid - a data-structure representing a map grid in the Nuggets game
 *
 * David Kotz, 2019
 */

#ifndef _GRID_H_
#define _GRID_H_

// the base grid comprises following characters
#define GRID_BLANK      ' '  // grid.c depends on this being space
#define GRID_ROOM_SPOT  '.'
#define GRID_PASS_SPOT  '#'
#define GRID_WALL_VERT  '|'
#define GRID_WALL_HORZ  '-'
#define GRID_WALL_CORN  '+'

// the occupants grid comprises the following characters, plus letters
#define GRID_PLAYER_ME  '@'
#define GRID_GOLD       '*'

/* smallest map possible is
 * +-+
 * |.|
 * +-+
 */
#define MinRows 3
#define MinCols 3

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "game.h"

/********************* types **************************/

typedef struct grid grid_t; // opaque type representing the grid

/********************* functions **********************/

grid_t* grid_new(const int nrows, const int ncols);
/* Create a new, empty grid.
 * Caller provides: size of grid (number of rows and columns).
 * Function returns: pointer to new grid object, or NULL if error.
 * Contract: caller must later call grid_delete on the new grid.
 */

grid_t* grid_fromString(const char* gridString);
/* Create a new grid and load its content from a string.
 * Caller provides: string representing a grid.
 * Function returns: pointer to new grid object, or NULL if error.
 * Contract:
 *   function copies the gridString provided by caller.
 *   caller must later call grid_delete on the new grid.
 * Notes:
 *   The size of the grid is inferred from the number of lines,
 *   and length of the lines, in the srting.  All lines must have
 *   the same length and the file must end with a newline.
 *   It is an error if the grid is smaller than MinRows x MinCols.
 */

grid_t* grid_fromFile(const char* filename);
/* Create a new grid and load its content from a mapfile.
 * Caller provides: filename for the mapfile.
 * Function returns: pointer to new grid object, or NULL if error.
 * Contract: caller must later call grid_delete on the new grid.
 * Notes: see grid_fromString.
 */

void grid_overlay(const grid_t* base, const grid_t* overlay,
                  const grid_t* mask, grid_t* out);
/* Overlay one grid on an another, producing the output.
 * Caller provides: pointers to four existing grids:
 *   the base grid, which is copied to the output grid;
 *   the overlay grid, whose non-empty cells may be copied to the output;
 *   the mask grid, which limits overlay to only its non-blank points,
 *   the output grid, which results from base + overlay.
 * The grids 'out'  and 'base' may be the same grid, updating 'base'.
 * The grids 'mask' and 'base' may be the same grid, meaning "no mask".
 * Function returns: nothing.
 * Notes:
 *   If any the four grids are NULL or of different size, no action is taken.
 */

void grid_visible(const grid_t* base, const int pr, const int pc, grid_t* out);
/* Construct in 'out' a subset of 'base' including only gridpoints
 * that are visible from point pr,pc.
 * Caller provides: pointers to two existing grids:
 *   the base grid, which is copied to the output grid;
 *   the output grid, whose non-blank points are a subset of those in base.
 *   A point pr,pc, from which visibility is computed.
 * Function returns: nothing.
 * Notes:
 *   If the two grids are not all the same size, or NULL, no action is taken.
 */

void grid_erase(grid_t* grid);
/* Erase the grid so it is all blank, as if it were a new grid.
 */

char grid_get(const grid_t* grid, const int r, const int c);
/* Return the character at a given gridpoint.
 * Caller provides: pointer to a grid; row number, column number.
 * Function returns: character at the given gridpoint,
 *   or null character if any error.
 */

void grid_set(grid_t* grid, const int r, const int c, const char x);
/* Set the character at the given gridpoint.
 * Caller provides: pointer to a grid; row/column number, new character.
 * Function returns: nothing.
 * Notes: if the grid is NULL, or row/column out of bounds, no action taken.
 */

int grid_nrows(const grid_t* grid);
/* Return the number of rows in the given grid.
 * Caller provides: pointer to an existing grid.
 * Function returns: number of rows, or zero if error.
 */

int grid_ncols(const grid_t* grid);
/* Return the number of columns in the given grid.
 * Caller provides: pointer to an existing grid.
 * Function returns: number of columns, or zero if error.
 */

const char* grid_string(const grid_t* grid);
/* Return string representation of the given grid.
 * Caller provides: pointer to an existing grid.
 * Function returns: pointer to string representing grid content,
 *   or NULL if error.
 * Notes: one string, in which each row is represented as a line of
 *   characters terminated by a newline character.
 */

void grid_delete(grid_t* grid);
/* Delete the given grid and free its memory.
 * Caller provides: pointer to an existing grid.
 * Function returns: nothing.
 * Notes: if the grid is NULL, no action is taken.
 */

bool grid_isSpot(const grid_t* grid, const int r, const int c);
/* Return true iff the given gridpoint is a "spot", that is,
 * it is either a room spot or a passage spot or a gold spot.
 */

bool grid_isGold(const grid_t* grid, const int r, const int c);
/* Return true iff the given gridpoint is a "gold spot".
 */

bool grid_isRoomSpot(const grid_t* grid, const int r, const int c);
/* Return true iff the given gridpoint is a "room spot".
 */

bool grid_isBlank(const grid_t* grid, const int r, const int c);
/* Return true iff the given gridpoint is GRID_BLANK.
 */

bool grid_isVisible(const grid_t* base,
                    const int r, const int c, const int pr, const int pc);
/* Is point r,c visible from point pr, pc?
 * Caller provides:
 *   pointer to the base grid (i.e., the map);
 *   point r,c,   which we want to know whether it is 'visible'
 *   point pr,pc, the point from which we determine visibility.
 * Function returns: true if visible, false otherwise or error.
 * See definition of 'visible' in REQUIREMENTS.md.
 */

#endif // _GRID_H_
