/*
 * grid - a data-structure representing a map grid in the Nuggets game
 *
 * David Kotz, 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "grid.h"
#include "../support/message.h" // only for message_MaxBytes

/**************** types ****************/

/* A "grid" is an array of characters, representing a two-dimensional matrix
 * in row-major order; each row has ncols characters plus a newline.
 * Thus there are nrows * (ncols+1) characters plus the terminating null.
 */
typedef struct grid grid_t;   // opaque type shared in grid.h
struct grid {                 // details of type, known inside this module
  int nrows, ncols;           // number of rows and columns
  char* cells;                // [grid_size(nrows, ncols) + 1]
};

// (char) a cell of the grid;
// by using a macro rather than a function, it is suitable as lvalue or rvalue
#define CELL(g,r,c) ((g)->cells[(r) * ((g)->ncols + 1) + (c)])

/**************** file-local global variables ****************/

/**************** local function prototypes ****************/
/* not visible outside this file */
static grid_t* grid_allocate(const int nrows, const int ncols);
static int grid_size(const int nrows, const int ncols);
static bool grid_sizesMatch(const grid_t* grid1, const grid_t* grid2);

/**************** grid_new ****************/
/* see grid.h for detailed interface description */
grid_t*
grid_new(const int nrows, const int ncols)
{
  // grid must be a minimum size
  if (nrows < MinRows || ncols < MinCols) {
    return NULL;
  }

  // allocate the grid
  grid_t* grid = grid_allocate(nrows, ncols);
  grid_erase(grid);
  return grid;
}

/**************** grid_fromString ****************/
/* see grid.h for detailed interface description */
grid_t*
grid_fromString(const char* gridString)
{
  // do some minimal validation before we create a grid object.

  // how many rows?  count the number of newlines
  int nrows = 0;
  for (const char* p = gridString; *p != '\0'; p++) {
    if (*p == '\n') {
      nrows++;
    }
  }

  if (nrows < MinRows) {
    fprintf(stderr, "grid_fromString: grid has %d rows, but minimum is %d\n",
            nrows, MinRows);
    return NULL;
  }

  // how many columns? (subtract one for the newline)
  int ncols = (strlen(gridString) / nrows) - 1;

  if (ncols < MinCols) {
    fprintf(stderr, "grid_fromString: grid has %d cols, but minimum is %d\n",
            ncols, MinCols);
    return NULL;
  }

  if (strlen(gridString) != grid_size(nrows, ncols)) {
    fprintf(stderr, "grid_fromString: string has %lu chars, but expected %d chars for a %d x %d grid\n",
            strlen(gridString), grid_size(nrows, ncols), nrows, ncols);
    return NULL;
  }

  // are all rows the same length?
  for (int row = 0; row < nrows; row++) {
    if (gridString[row * (ncols + 1) + ncols] != '\n') {
      // some row is too long or too short
      fprintf(stderr, "grid_fromString: rows are not the same width\n");
      return NULL;
    }
  }

  // allocate the grid and copy in the gridString
  grid_t* grid = grid_allocate(nrows, ncols);
  if (grid != NULL) {
    strcpy(grid->cells, gridString);
  }
  return grid;
}

/**************** grid_fromFile ****************/
/* see grid.h for detailed interface description */
grid_t*
grid_fromFile(const char* filename)
{
  if (filename == NULL) {
    return NULL;
  }

  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    return NULL;
  }

  // since any real grid must fit into a message,
  // we can allocate and try to read that max-size grid.
  char gridString[message_MaxBytes];
  // read and null-terminate string from the file
  size_t nChars = fread(gridString, sizeof(char), message_MaxBytes, fp);
  gridString[nChars] = '\0';
  fclose(fp);

  return grid_fromString(gridString);
}

/**************** grid_overlay ****************/
/* see grid.h for detailed interface description */
void grid_overlay(const grid_t* base, const grid_t* overlay,
                  const grid_t* mask, grid_t* out)
{
  // check grids and sizes - they must all be non-NULL and the same size
  if (!grid_sizesMatch(base, overlay)) return;
  if (!grid_sizesMatch(base, out))     return;
  if (!grid_sizesMatch(base, mask))    return;

  // copy to output from base or overlay, accordingly.
  const int nrows = base->nrows;
  const int ncols = base->ncols;
  for (int r = 0; r < nrows; r++) {
    for (int c = 0; c < ncols; c++) {
      // We can overlay if the overlay character is non-blank
      // and the mask character is non-blank.
      if (!grid_isBlank(overlay, r, c) && !grid_isBlank(mask, r, c)) {
        CELL(out, r, c) = CELL(overlay, r, c);
      } else {
        CELL(out, r, c) = CELL(base, r, c);
      }
    }
  }
}

/**************** grid_visible ****************/
/* see grid.h for detailed interface description */
/* NOTE: it is ok for base and out to be the same grid. */
void
grid_visible(const grid_t* base, const int pr, const int pc, grid_t* out)
{
  // check grid sizes - they must both be non-NULL and the same size
  if (grid_sizesMatch(base, out)) {
    const int nrows = base->nrows;
    const int ncols = base->ncols;

    // copy the visible cells from base grid to output grid
    for (int r = 0; r < nrows; r++) {
      for (int c = 0; c < ncols; c++) {
        if (grid_isVisible(base, r, c, pr, pc)) {
          CELL(out, r, c) = CELL(base, r, c);
        } else {
          CELL(out, r, c) = GRID_BLANK;
        }
      }
    }
  }
}

/**************** grid_erase ****************/
/* see grid.h for detailed interface description */
void
grid_erase(grid_t* grid)
{
  if (grid != NULL) {
    const int nrows = grid->nrows;
    const int ncols = grid->ncols;
    // initialize each row vector, full of spaces, newline terminated
    char* p = grid->cells;         // pointer to the beginning of a row
    for (int r = 0; r < nrows; r++, p += ncols+1) {
      sprintf(p, "%*c\n", ncols, ' ');
    }
  }
}

/**************** grid_get ****************/
/* see grid.h for detailed interface description */
char
grid_get(const grid_t* grid, const int r, const int c)
{
  if (grid != NULL) {
    if ((r >= 0 && r < grid->nrows) && (c >= 0 && c < grid->ncols)) {
      return CELL(grid, r, c);
    }
  }
  return '\0';
}

/**************** grid_set ****************/
/* see grid.h for detailed interface description */
void
grid_set(grid_t* grid, const int r, const int c, const char x)
{
  if (grid != NULL) {
    if ((r >= 0 && r < grid->nrows) && (c >= 0 && c < grid->ncols)) {
      CELL(grid, r, c) = x;
    }
  }
}

/**************** grid_nrows ****************/
/* see grid.h for detailed interface description */
int
grid_nrows(const grid_t* grid)
{
  if (grid != NULL) {
    return grid->nrows;
  } else {
    return 0;
  }
}

/**************** grid_ncols ****************/
/* see grid.h for detailed interface description */
int
grid_ncols(const grid_t* grid)
{
  if (grid != NULL) {
    return grid->ncols;
  } else {
    return 0;
  }
}

/**************** grid_string ****************/
/* see grid.h for detailed interface description */
const char*
grid_string(const grid_t* grid)
{
  if (grid == NULL) {
    return NULL;
  } else {
    return grid->cells;
  }
}

/**************** grid_delete ****************/
/* see grid.h for detailed interface description */
void
grid_delete(grid_t* grid)
{
  if (grid != NULL) {
    if (grid->cells != NULL) {
      free(grid->cells); // the cells
    }
    free(grid); // the struct
  }
}

/**************** grid_isSpot ****************/
/* see grid.h for detailed interface description */
/* is point r,c a spot? */
bool
grid_isSpot(const grid_t* grid, const int r, const int c)
{
  return grid == NULL ? false :
       CELL(grid, r, c) == GRID_ROOM_SPOT
    || CELL(grid, r, c) == GRID_PASS_SPOT
    || CELL(grid, r, c) == GRID_GOLD
    || isalpha(CELL(grid, r, c));
}

/**************** grid_isRoomSpot ****************/
/* see grid.h for detailed interface description */
/* is point r,c a room spot? */
bool
grid_isRoomSpot(const grid_t* grid, const int r, const int c)
{
  return grid == NULL ? false :
    CELL(grid, r, c) == GRID_ROOM_SPOT ||
    CELL(grid, r, c) == GRID_GOLD ||
    isalpha(CELL(grid, r, c));
}

/**************** grid_isGold ****************/
/* see grid.h for detailed interface description */
/* is point r,c a spot? */
bool
grid_isGold(const grid_t* grid, const int r, const int c)
{
  return grid == NULL ? false :
    CELL(grid, r, c) == GRID_GOLD;
}

/**************** grid_isGold ****************/
/* see grid.h for detailed interface description */
/* is point r,c a spot? */
bool
grid_isPlayer(const grid_t* grid, const int r, const int c)
{
  return grid == NULL ? false :
    isalpha(CELL(grid, r, c));
}

/**************** grid_isBlank ****************/
/* see grid.h for detailed interface description */
/* is point r,c a GRID_BLANK? */
bool
grid_isBlank(const grid_t* grid, const int r, const int c)
{
  return grid == NULL ? false : CELL(grid, r, c) == GRID_BLANK;
}

/**************** grid_isVisible ****************/
/* see grid.h for detailed interface description */
/* is point r,c visible from point pr, pc? */
bool
grid_isVisible(const grid_t* base,
               const int r, const int c, const int pr, const int pc)
{
  if (base == NULL) {
    return false;
  }

  // optimization: blank spots are never visible
  if (CELL(base, r, c) == GRID_BLANK) {
    return false;
  }

  // How does the destination point (r,c) differ from player point (pr,pc)?
  const int rdelta = r-pr;                  // row delta
  const int cdelta = c-pc;                  // col delta
  const int rsign = (rdelta < 0) ? -1 : +1; // sign of row delta
  const int csign = (cdelta < 0) ? -1 : +1; // sign of col delta

  // Four cases: same gridpoint, vertical line, horizontal line, sloping line;
  // return false if anything blocks our vision, otherwise true.

  if (rdelta == 0 && cdelta == 0) {       // 1. same gridpoint
    return true;
  } else if (cdelta == 0) {               // 2. vertical line
    // step along the path from pr,c to r,c, and see if we hit a wall
    for (int row = pr + rsign; row != r; row += rsign) {
      if (!grid_isRoomSpot(base, row, c)) {
        return false;
      }
    }
    return true;
  } else if (rdelta == 0) {               // 3. horizontal line
    // step along the path from r,pc to r,c, and see if we hit a wall
    for (int col = pc + csign; col != c; col += csign) {
      if (!grid_isRoomSpot(base, r, col)) {
        return false;
      }
    }
    return true;
  } else {                                // 4. sloping line
    const float slope = (float) rdelta / (float) cdelta;

    // step along rows
    for (int row = pr + rsign; row != r; row += rsign) {
      float colcept = pc + (float)(row - pr) / slope; // intercept
      if (   !grid_isRoomSpot(base, row, (int) floor(colcept))
          && !grid_isRoomSpot(base, row, (int) ceil(colcept))) {
        return false;
      }
    }
    // step along cols
    for (int col = pc + csign; col != c; col += csign) {
      float rowcept = pr + slope * (float)(col - pc); // intercept
      if (   !grid_isRoomSpot(base, (int) floor(rowcept), col)
          && !grid_isRoomSpot(base, (int) ceil(rowcept), col)) {
        return false;
      }
    }
    return true;
  }
}

/**************** grid_allocate ****************/
/* INTERNAL FUNCTION:
 *  Allocate the memory needed for the grid, but do not initialize
 *  the characters within the grid.  Used by grid_new and grid_fromString.
 */
static grid_t*
grid_allocate(const int nrows, const int ncols)
{
  // allocate the struct
  grid_t* grid = malloc(sizeof(grid_t));
  if (grid == NULL) {
    return NULL;
  }

  // initialize the members
  grid->nrows = nrows;
  grid->ncols = ncols;
  grid->cells = calloc(grid_size(nrows, ncols)+1, sizeof(char));

  if (grid->cells == NULL) {
    free(grid);
    return NULL;
  }

  return grid;
}

/**************** grid_size ****************/
/* INTERNAL FUNCTION:
 * Return number of chars in a grid, NOT including terminating null.
 * Caller provides: pointer to an existing grid.
 * Function returns: number of chars in a grid, NOT including terminating null.
 * Notes: size includes each row's terminating newline.
 */
static int
grid_size(const int nrows, const int ncols)
{
  // Return number of chars in a grid, NOT including terminating null.
  // There are several rows (nrows);
  // each row has ncols chars plus a terminating newline (ncols + 1)
  return nrows * (ncols + 1);
}

/**************** grid_sizesMatch ****************/
/* INTERNAL FUNCTION: Are the two grids the same size?
 * Caller provides: pointers to two existing grids.
 * Function returns: true iff both are non-NULL and the same nrows x ncols.
 */
static bool
grid_sizesMatch(const grid_t* grid1, const grid_t* grid2)
{
  if (grid1 == NULL || grid2 == NULL) {
    return false;
  } else {
    return (grid1->nrows == grid2->nrows && grid1->ncols == grid2->ncols);
  }
}


/* ******************************************************************* */
/* ******************************************************************* */

/**************** UNIT TEST ****************/

#ifdef UNIT_TEST

static int arg2int(const char* progname, char* arg);
int test2(const int argc, char* argv[]);
int test3(const int argc, char* argv[]);

/*
 * usage: one of
 *   gridtest nrows ncols > grid.txt
 *   gridtest filename.txt
 */
int
main(const int argc, char* argv[])
{
  switch (argc) {
  case 2:
    return test2(argc, argv);
  case 3:
    return test3(argc, argv);
  default:
    fprintf(stderr, "usage: %s nrows ncols\n", argv[0]);
    fprintf(stderr, "   or: %s filename\n", argv[0]);
    return 1;
  }
}

/* argc == 2:  gridtest filename */
int
test2(const int argc, char* argv[])
{
  const char* progname = argv[0];
  const char* filename = argv[1];

  // load the grid
  grid_t* grid = grid_fromFile(filename);
  if (grid == NULL) {
    fprintf(stderr, "%s: grid_fromFile failed\n", progname);
    exit(4);
  }

  // print the grid
  printf("\noriginal grid: %d x %d\n==========================\n",
         grid_nrows(grid), grid_ncols(grid));
  fputs(grid_string(grid), stdout);

  // delete the grid
  grid_delete(grid);

  return 0;
}

/* argc == 3:  gridtest nrows ncols */
int
test3(const int argc, char* argv[])
{
  const char* progname = argv[0];

  // number of rows and cols for the test grid
  const int nrows = arg2int(progname, argv[1]);
  const int ncols = arg2int(progname, argv[2]);

  // make a new grid
  grid_t* base = grid_new(nrows, ncols);
  if (base == NULL) {
    fprintf(stderr, "%s: grid_new failed\n", progname);
    exit(4);
  }

  // fill all cells of the grid
  int let = 0;
  for (int r = 0; r < nrows; r++) {
    for (int c = 0; c < ncols; c++) {
      grid_set(base, r, c, 'a' + let++ % 26);
    }
  }

  // print the base grid
  printf("\nbase grid: %d x %d\n==========================\n",
         grid_nrows(base), grid_ncols(base));
  fputs(grid_string(base), stdout);

  // make an overlay grid
  grid_t* overlay = grid_new(nrows, ncols);

  // fill some diagonal cells of the grid
  for (int r = 0; r < nrows; r++) {
    grid_set(overlay, r, r % ncols, '.');
    grid_set(overlay, r, ncols - r % ncols - 1, '.');
  }

  // print the overlay grid
  printf("\noverlay grid: %d x %d\n==========================\n",
         grid_nrows(overlay), grid_ncols(overlay));
  fputs(grid_string(overlay), stdout);

  // overlay onto the base
  grid_t* blend = grid_new(nrows, ncols);
  grid_overlay(base, overlay, base, blend);

  // check those cells of the grid
  for (int r = 0; r < nrows; r++) {
    char cell1 = grid_get(blend, r, r % ncols);
    if (cell1 != '.') {
      fprintf(stderr, "mismatch; cell[%d][%d] == '%c'\n", r, r % ncols, cell1);
    }
    char cell2 = grid_get(blend, r, ncols - r % ncols - 1);
    if (cell2 != '.') {
      fprintf(stderr, "mismatch; cell[%d][%d] == '%c'\n", r, ncols - r % ncols - 1, cell2);
    }
  }

  // print the grid
  printf("\nblend grid with no mask: %d x %d\n==========================\n",
         grid_nrows(blend), grid_ncols(blend));
  fputs(grid_string(blend), stdout);

  // now use the overlay as a mask
  grid_t* blank = grid_new(nrows, ncols);
  grid_overlay(blank, base, overlay, blend);

  // print the grid
  printf("\nblend grid with overlay as mask: %d x %d\n====================\n",
         grid_nrows(blend), grid_ncols(blend));
  fputs(grid_string(blend), stdout);

  // delete the grids
  grid_delete(base);
  grid_delete(overlay);
  grid_delete(blend);
  grid_delete(blank);

  return 0;
}

/* convert an argument to an integer, and complain/exit if error */
static int arg2int(const char* progname, char* arg)
{
  int number;
  char excess;

  if (sscanf(arg, "%d%c", &number, &excess) == 1) {
    return number;
  } else {
    fprintf(stderr, "%s: bad integer '%s'\n", progname, arg);
    exit(2);
  }
}

#endif // UNIT_TEST
