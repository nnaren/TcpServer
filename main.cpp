
#include"socket_helper.h"

int main(int argc,char** argv)
{
    
    unsigned short port = PORT ;
    int serverfd = ReadyforTcpServer(port);
    SetSockNonBlock(serverfd);
    AcceptBySelect(serverfd);
    return 0;

}