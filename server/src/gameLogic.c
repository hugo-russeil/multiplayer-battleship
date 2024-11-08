#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "../include/gameLogic.h"
#include "../include/tuple.h"
#include "../include/grid.h"
#include "../include/ship.h"
#include "../include/network.h"

void initGrids(struct Cell grid1[GRID_SIZE][GRID_SIZE], struct Cell grid2[GRID_SIZE][GRID_SIZE]) {
    for(int i = 0; i < GRID_SIZE ; i++){
        for(int j = 0; j < GRID_SIZE; j++){
            grid1[i][j].aShip = NONE;
            grid1[i][j].aState = UNSHOT;
            grid2[i][j].aShip = NONE;
            grid2[i][j].aState = UNSHOT;
        }
    }
}

void placeShips(struct Cell grid1[GRID_SIZE][GRID_SIZE], struct Cell grid2[GRID_SIZE][GRID_SIZE]) {
    for(int gridIndex = 0; gridIndex < 2; gridIndex++) {
        struct Cell (*grid)[GRID_SIZE][GRID_SIZE] = gridIndex == 0 ? &grid1 : &grid2;
        for(int shipType = CARRIER; shipType >= DESTROYER; shipType--) {
            for(int shipCount = 0; shipCount < (shipType == SUBMARINE ? 2 : 1); shipCount++) {
                bool placed = false;
                while(!placed) {
                    int x = rand() % GRID_SIZE;
                    int y = rand() % GRID_SIZE;
                    int direction = rand() % 4; // 0: right, 1: down, 2: left, 3: up
                    placed = true;
                    for(int i = 0; i < shipType; i++) {
                        int shipX = x + (direction == 0 ? i : direction == 2 ? -i : 0);
                        int shipY = y + (direction == 1 ? i : direction == 3 ? -i : 0);
                        if(shipX < 0 || shipX >= GRID_SIZE || shipY < 0 || shipY >= GRID_SIZE || (*grid)[shipX][shipY].aShip != NONE) {
                            placed = false;
                            break;
                        }
                    }
                    if(placed) {
                        for(int i = 0; i < shipType; i++) {
                            int shipX = x + (direction == 0 ? i : direction == 2 ? -i : 0);
                            int shipY = y + (direction == 1 ? i : direction == 3 ? -i : 0);
                            (*grid)[shipX][shipY].aShip = shipType;
                        }
                    }
                }
            }
        }
    }
}

void printGrid(struct Cell grid[GRID_SIZE][GRID_SIZE]) {
    for(int i = 0; i < GRID_SIZE ; i++){
        for(int y = 0; y < GRID_SIZE; y++){
            if(grid[i][y].aShip == NONE && grid[i][y].aState == UNSHOT){
                printf("%c ", grid[i][y].aState);
            }
            else printf("%d ", grid[i][y].aShip);
        }
        printf("\n");
    }
    printf("\n");
}

void gameLoop(tuple direction[4], int nbShips, char* argv[]) {
    struct sockaddr_in client1, client2; //client SAPs
        socklen_t len1=sizeof(client1), len2=sizeof(client2);
        int sock_pipe1, sock_pipe2; //communication sockets
        int ret;

        sock_pipe1 = accept(sock, (struct sockaddr *) &client1, &len1);
        sock_pipe2 = accept(sock, (struct sockaddr *) &client2, &len2);


        int f = fork();
        if(f == 0){
            // preparing grids
            struct Cell grid1[GRID_SIZE][GRID_SIZE], grid2[GRID_SIZE][GRID_SIZE];
            initGrids(grid1, grid2);

            srand(time(NULL));

            int dir, x, y, testX, testY, nbCells;
            bool test;
            enum Ship shipToPlace;
            // Place ships on grids
            placeShips(grid1, grid2);

            //printing grids
            printGrid(grid1);
            printGrid(grid2);
 
            bool win = false;
            int nbShipSunk1 = 0, nbShipSunk2 = 0;
            while (!win) {

                handleClientCommunication(sock_pipe1, client1, grid2, &nbShipSunk2, &win, direction, nbShips, argv);

                handleClientCommunication(sock_pipe2, client2, grid1, &nbShipSunk1, &win, direction, nbShips, argv);


                sleep(2);
            }
            close(sock_pipe1);
            close(sock_pipe2);
        }
        close(sock_pipe1);
        close(sock_pipe2);
}