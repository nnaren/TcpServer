
#include"socket_helper.h"
#include <unistd.h>
#include <stdio.h>
int main(int argc,char** argv)
{
    
    unsigned short port = PORT ;
    int serverfd = ReadyforTcpServer(port,1);
    printf("ok\n");
    AcceptBySelect(serverfd);
    close(serverfd);
    return 0;

}