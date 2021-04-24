#define BUF_SIZE 1024
#define PORT 8900

#include <pthread.h>
struct ClientSocketInfo
{
    /* data */
    int clientfd;
    struct sockaddr_in* clntAddr;
};

int ReadyforTcpServer(unsigned short port,int blockFlag);

void SetSockNonBlock(int socketfd);

void AcceptByMultiThread(int serverfd, pthread_t pid);
void AcceptNormal(int serverfd);
void AcceptBySelect(int serverfd);
