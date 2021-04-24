
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>


void SetSockNonBlock(int socketfd) 
{
    int flags;
    flags = fcntl(socketfd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl(F_GETFL) failed");
        exit(EXIT_FAILURE);
    }
    if (fcntl(socketfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl(F_SETFL) failed");
        exit(EXIT_FAILURE);
    }
}
