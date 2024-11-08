#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

char* id=0;
short sport=0;

int sock=0;	/* communication socket */

int main(int argc, char** argv)
{
    struct  sockaddr_in moi; /* client SAP */
    struct  sockaddr_in server; /* server SAP */
    int ret,len;

    //handling arguments-related errors
    if (argc!=4) {
        fprintf(stderr,"usage: %s id server port\n",argv[0]);
        exit(1);
    }
    id= argv[1];
    sport= atoi(argv[3]);

    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr,"%s: socket %s\n",argv[0],
                strerror(errno));
        exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(sport);
    inet_aton(argv[2],&server.sin_addr);

    if (connect(sock, (struct sockaddr *) &server, sizeof(server))<0) {
        fprintf(stderr,"%s: connect %s\n",argv[0],strerror(errno));
        perror("bind");
        exit(1);
    }
    len=sizeof(moi);
    getsockname(sock,(struct  sockaddr  *)&moi,&len);

    while(1){
        char    buf_read[1<<8], buf_write[1<<8];
        int x, y, rep;
        do{
            printf("\nVeuillez entrer les coordonnées de tir (x , y):");
            rep = scanf("%d %d", &x, &y);
        }while(rep != 2);

        // sending id to server
        sprintf(buf_write,"(%d %d)",x, y);
        // sending coordinates to server
        ret=write(sock, buf_write, strlen(buf_write));
        if (ret<strlen(buf_write)) {
            printf("\n%s: error in write (num=%d, mess=%s)\n",
                   argv[0],ret,strerror(errno));
            continue;
        }

        // server response
        ret=read(sock, buf_read, 256);

        if (ret<=0) {
            printf("%s: read=%d: %s\n",argv[0],ret,
                   strerror(errno));
            break;
        }
        buf_read[ret] = '\0'; // Null-terminate the buffer

        // Strip newline character from server's response
        char *pos;
        if ((pos=strchr(buf_read, '\n')) != NULL)
            *pos = '\0';
        system("clear");
        printf("\n%s\n", buf_read);
        memset(buf_read, 0, 256);

        // Send confirmation to server
        ret=write(sock, "1", 1);
        if (ret<0) {
            printf("\n%s: error in write (num=%d, mess=%s)\n",
                   argv[0],ret,strerror(errno));
            continue;
        }

        // Read grid from server
        ret=read(sock, buf_read, 256);

        if (ret<=0) {
            printf("%s: read=%d: %s\n",argv[0],ret,
                   strerror(errno));
            break;
        }
        buf_read[ret] = '\0'; // Null-terminate the buffer
        printf("\n%s\n", buf_read);


    }
    close(sock);

    return 0;
}