#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "grid.h"
#include "ship.h"
#include "tuple.h"



void initGrids(struct Cell grid1[GRID_SIZE][GRID_SIZE], struct Cell grid2[GRID_SIZE][GRID_SIZE]);
void placeShips(struct Cell grid1[GRID_SIZE][GRID_SIZE], struct Cell grid2[GRID_SIZE][GRID_SIZE]);
void printGrid(struct Cell grid[GRID_SIZE][GRID_SIZE]);
void gameLoop(tuple direction[4], int nbShips, char* argv[]);

#endif  // GAME_LOGIC_H