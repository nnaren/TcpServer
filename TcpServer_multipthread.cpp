#include "socket_helper.h"
#include "ptheard_y.h"
#include<stdlib.h>

#include<unistd.h>
#define NUM_PTHREAD 5


void *handle_clnt(void *arg);
int main(int argc, char *argv[])
{
    int serverfd, clntfd;
    struct sockaddr_in  clnt_adr;
    socklen_t clnt_adr_sz = sizeof(clnt_adr);;
    pthread_t pid;
    // if (argc != 2)
    // {
    //     printf("Usage : %s <port>\n", argv[0]);
    //     exit(1);
    // }

    // pthread_mutex_init(&mutx, NULL); //创建互斥锁
    serverfd = ReadyforTcpServer(10086);
    printf("服务端fd: %d\n", serverfd);
    if (serverfd == -1)
    {
        /* code */
        return 0;
    }

    close(serverfd);
    return 0;
    
}

