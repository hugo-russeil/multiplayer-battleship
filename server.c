#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "typeDef.h"

char* id=0;
short port=0;

int sock = 0; //communication socket

char* gridToString(struct Cell grid[10][10]) {
    static char gridStr[201]; // 100 cells + 100 spaces + 20 newline characters + 1 null terminator
    int index = 0;
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            gridStr[index++] = grid[i][j].aState;
            gridStr[index++] = ' '; // Adding a space after each character
        }
        gridStr[index++] = '\n';
    }
    gridStr[index] = '\0';
    return gridStr;
}

int main(int argc, char** argv)
{
    tuple right; right.x = 1; right.y = 0;
    tuple left; left.x = -1; left.y = 0;
    tuple up; up.x = 0; up.y = -1;
    tuple down; down.x = 0; down.y = 1;
    tuple direction[4] = {right, left, up, down};

    int nbShips = 8;

    struct  sockaddr_in server; //server SAP

    if (argc!=3) {
        fprintf(stderr,"usage: %s id port\n",argv[0]);
        exit(1);
    }
    id= argv[1];
    port= atoi(argv[2]);

    // Create socket
    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr,"%s: socket %s\n",argv[0],strerror(errno));
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind (sock, (struct sockaddr *) &server, sizeof(server))<0) {
        fprintf(stderr,"%s: bind %s\n",argv[0],strerror(errno));
        exit(1);
    }

// Listen on socket
    if (listen (sock, 5)!=0) {
        fprintf(stderr,"%s: listen %s\n",argv[0],strerror(errno));
        exit(1);
    }

    while (1) {
        struct sockaddr_in client1, client2; //client SAPs
        socklen_t len1=sizeof(client1), len2=sizeof(client2);
        int sock_pipe1, sock_pipe2; //communication sockets
        int ret;

        sock_pipe1 = accept(sock, (struct sockaddr *) &client1, &len1);
        sock_pipe2 = accept(sock, (struct sockaddr *) &client2, &len2);


        int f = fork();
        if(f == 0){
            // preparing grids
            /*------------------------------------------*/
            struct Cell grid1[10][10], grid2[10][10];
            for(int i = 0; i < 10 ; i++){
                for(int j = 0; j < 10; j++){
                    grid1[i][j].aShip = NONE;
                    grid1[i][j].aState = UNSHOT;
                    grid2[i][j].aShip = NONE;
                    grid2[i][j].aState = UNSHOT;
                }
            }

            srand(time(NULL));

            int dir, x, y, testX, testY, nbCells;
            bool test;
            enum Ship shipToPlace;
            // Place ships on grids
            for(int gridIndex = 0; gridIndex < 2; gridIndex++) {
                struct Cell (*grid)[10][10] = gridIndex == 0 ? &grid1 : &grid2;
                for(int shipType = CARRIER; shipType >= DESTROYER; shipType--) {
                    for(int shipCount = 0; shipCount < (shipType == SUBMARINE ? 2 : 1); shipCount++) {
                        bool placed = false;
                        while(!placed) {
                            int x = rand() % 10;
                            int y = rand() % 10;
                            int direction = rand() % 4; // 0: right, 1: down, 2: left, 3: up
                            placed = true;
                            for(int i = 0; i < shipType; i++) {
                                int shipX = x + (direction == 0 ? i : direction == 2 ? -i : 0);
                                int shipY = y + (direction == 1 ? i : direction == 3 ? -i : 0);
                                if(shipX < 0 || shipX >= 10 || shipY < 0 || shipY >= 10 || (*grid)[shipX][shipY].aShip != NONE) {
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

            //printing grids
            for(int i = 0; i < 10 ; i++){
                for(int y = 0; y < 10; y++){
                    if(grid1[i][y].aShip == NONE && grid1[i][y].aState == UNSHOT){
                        printf("%c ", grid1[i][y].aState);
                    }
                    else printf("%d ", grid1[i][y].aShip);
                }
                printf("\n");
            }
            printf("\n");

            for(int i = 0; i < 10 ; i++){
                for(int y = 0; y < 10; y++){
                    if(grid2[i][y].aShip == NONE && grid2[i][y].aState == UNSHOT){
                        printf("%c ", grid2[i][y].aState);
                    }
                    else printf("%d ", grid2[i][y].aShip);
                }
                printf("\n");
            }
            printf("\n");
            /*------------------------------------------*/
            bool win = false;
            int nbShipSunk1 = 0, nbShipSunk2 = 0;
            while (!win) {
                char buf_read1[1<<8], buf_write1[1<<8];
                char buf_read2[1<<8], buf_write2[1<<8];
                // coordinates from client
                ret=read(sock_pipe1, buf_read1, 256);
                // handling errors related to reading from client
                if (ret<=0) {
                    printf("%s: read=%d: %s\n",argv[0],ret,
                           strerror(errno));
                    break;
                }

                printf("server %s received from client 1 (%s,%4d) : %s\n", id, inet_ntoa(client1.sin_addr), ntohs(client1.sin_port),buf_read1);
                int tirX = 0;
                int tirY = 0;
                sscanf(buf_read1, "(%d %d)", &tirX, &tirY);
                tirX--;
                tirY--;

                if(!(tirX >= 10 || tirY >= 10 || tirX < 0 || tirY < 0)){
                    if(grid2[tirX][tirY].aState == UNSHOT){
                        if(grid2[tirX][tirY].aShip == NONE){
                            grid2[tirX][tirY].aState = MISS;
                            //responding to client
                            sprintf(buf_write1,"Miss\n");
                        }
                        else{
                            grid2[tirX][tirY].aState = HIT;

                            //checking if boat is sunk
                            int orientation = 0;
                            for(int i = 0; i < 4; i++){
                                if(grid2[tirX + direction[i].x][tirY + direction[i].y].aShip == grid2[tirX][tirY].aShip){
                                    orientation = i;
                                }
                            }
                            int tirXBis = tirX;
                            int tirYBis = tirY;
                            int counter = 0;
                            while(grid2[tirXBis - direction[orientation].x][tirYBis - direction[orientation].y].aShip == grid2[tirX][tirY].aShip){
                                tirXBis -= direction[orientation].x;
                                tirYBis -= direction[orientation].y;
                            }
                            for(int i = 0; i < grid2[tirX][tirY].aShip; i++){
                                if(grid2[tirXBis][tirYBis].aState == HIT){
                                    counter++;
                                }
                                tirXBis += direction[orientation].x;
                                tirYBis += direction[orientation].y;
                            }
                            if(counter == grid2[tirX][tirY].aShip){
                                nbShipSunk2 ++;
                                // testing if all ships are sunk
                                if(nbShipSunk2 == nbShips){
                                    win = true;
                                    sprintf(buf_write1,"Hit, sunk\n You won!\n");
                                }
                                else{
                                    //responding to client
                                    sprintf(buf_write1,"Hit, sunk !\n");
                                }

                                //change the state of cells to SUNK where a boat is sunk
                                tirXBis = tirX;
                                tirYBis = tirY;
                                for(int i = 0; i < grid2[tirX][tirY].aShip; i++){
                                    grid2[tirXBis][tirYBis].aState = SUNK;
                                    tirXBis += direction[orientation].x;
                                    tirYBis += direction[orientation].y;
                                }
                            }
                            else{
                                //responding to client
                                sprintf(buf_write1, "Hit !\n");
                            }
                        }
                    }
                    else{
                        //responding to client
                        sprintf(buf_write1,"%s","You already shot there\n");
                    }
                }
                else{
                    //responding to client
                    sprintf(buf_write1,"%s","Invalid Coordinates\n");
                }

                //sending response to client
                ret=write(sock_pipe1, buf_write1, strlen(buf_write1));

                char confirm_buf[1];
                //Wait for confirmation from client
                ret=read(sock_pipe1, confirm_buf, 1);
                if (ret<=0) {
                    printf("%s: read=%d: %s\n",argv[0],ret,
                           strerror(errno));
                    continue;
                }

                //Check if anything was received from the client
                if (ret > 0) {
                    printf("Received confirmation from client\n");
                } else {
                    printf("Did not receive confirmation from client\n");
                    continue;
                }

                //sending grid to client
                char* gridStr2 = gridToString(grid2);
                ret=write(sock_pipe1, gridStr2, strlen(gridStr2));

                if (ret<=0) {
                    printf("%s: write=%d: %s\n",argv[0],ret,
                           strerror(errno));
                    break;
                }

                //printing grid
                for(int i = 0; i < 10 ; i++){
                    for(int y = 0; y < 10; y++){
                        if(grid2[i][y].aShip == NONE || grid2[i][y].aState != UNSHOT){
                            printf("%c ", grid2[i][y].aState);
                        }
                        else printf("%d ", grid2[i][y].aShip);
                    }
                    printf("\n");
                }
                printf("\n");

                // coordinates from client2
                ret=read(sock_pipe2, buf_read2, 256);

                // handling errors related to reading from client
                if (ret<=0) {
                    printf("%s: read=%d: %s\n",argv[0],ret,
                           strerror(errno));
                    break;
                }

                printf("server %s received from client 2 (%s,%4d) : %s\n", id, inet_ntoa(client2.sin_addr), ntohs(client2.sin_port),buf_read2);
                tirX = 0;
                tirY = 0;
                sscanf(buf_read2, "(%d %d)", &tirX, &tirY);
                tirX--;
                tirY--;

                if(!(tirX >= 10 || tirY >= 10 || tirX < 0 || tirY < 0)){
                    if(grid1[tirX][tirY].aState == UNSHOT){
                        if(grid1[tirX][tirY].aShip == NONE){
                            grid1[tirX][tirY].aState = MISS;
                            //responding to client
                            sprintf(buf_write2,"Miss\n");
                        }
                        else{
                            grid1[tirX][tirY].aState = HIT;

                            //checking if boat is sunk
                            int orientation = 0;
                            for(int i = 0; i < 4; i++){
                                if(grid1[tirX + direction[i].x][tirY + direction[i].y].aShip == grid1[tirX][tirY].aShip){
                                    orientation = i;
                                }
                            }
                            int tirXBis = tirX;
                            int tirYBis = tirY;
                            int counter = 0;
                            while(grid1[tirXBis - direction[orientation].x][tirYBis - direction[orientation].y].aShip == grid1[tirX][tirY].aShip){
                                tirXBis -= direction[orientation].x;
                                tirYBis -= direction[orientation].y;
                            }
                            for(int i = 0; i < grid1[tirX][tirY].aShip; i++){
                                if(grid1[tirXBis][tirYBis].aState == HIT){
                                    counter++;
                                }
                                tirXBis += direction[orientation].x;
                                tirYBis += direction[orientation].y;
                            }
                            if(counter == grid1[tirX][tirY].aShip){
                                nbShipSunk2 ++;
                                // testing if all ships are sunk
                                if(nbShipSunk2 == nbShips){
                                    win = true;
                                    sprintf(buf_write2,"Hit, sunk\n You won!\n");
                                }
                                else{
                                    //responding to client
                                    sprintf(buf_write2,"Hit, sunk !\n");
                                }

                                //change the state of cells to SUNK where a boat is sunk
                                tirXBis = tirX;
                                tirYBis = tirY;
                                for(int i = 0; i < grid1[tirX][tirY].aShip; i++){
                                    grid1[tirXBis][tirYBis].aState = SUNK;
                                    tirXBis += direction[orientation].x;
                                    tirYBis += direction[orientation].y;
                                }
                            }
                            else{
                                //responding to client
                                sprintf(buf_write2, "Hit !\n");
                            }
                        }
                    }
                    else{
                        //responding to client
                        sprintf(buf_write2,"%s","You already shot there\n");
                    }
                }
                else{
                    //responding to client
                    sprintf(buf_write2,"%s","Invalid Coordinates\n");
                }

                //sending response to client
                ret=write(sock_pipe2, buf_write2, strlen(buf_write2));

                //char confirm_buf[1];
                //Wait for confirmation from client
                ret=read(sock_pipe2, confirm_buf, 1);
                if (ret<=0) {
                    printf("%s: read=%d: %s\n",argv[0],ret,
                           strerror(errno));
                    continue;
                }

                //Check if anything was received from the client
                if (ret > 0) {
                    printf("Received confirmation from client\n");
                } else {
                    printf("Did not receive confirmation from client\n");
                    continue;
                }

                //sending grid to client
                char* gridStr1 = gridToString(grid1);
                ret=write(sock_pipe2, gridStr1, strlen(gridStr1));

                if (ret<=0) {
                    printf("%s: write=%d: %s\n",argv[0],ret,
                           strerror(errno));
                    break;
                }

                //printing grid
                for(int i = 0; i < 10 ; i++){
                    for(int y = 0; y < 10; y++){
                        if(grid1[i][y].aShip == NONE || grid1[i][y].aState != UNSHOT){
                            printf("%c ", grid1[i][y].aState);
                        }
                        else printf("%d ", grid1[i][y].aShip);
                    }
                    printf("\n");
                }
                printf("\n");

                sleep(2);
            }
            close(sock_pipe1);
            close(sock_pipe2);
        }
        close(sock_pipe1);
        close(sock_pipe2);
    }
}