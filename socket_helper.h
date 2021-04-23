#include<sys/socket.h>
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>

#define DEFAULT_PORT 10086
#define BUF_SIZE 1024

struct ClientSocketInfo
{
    /* data */
    int clientfd;
    struct sockaddr_in* clntAddr;
};

int ReadyforTcpServer(unsigned short port)
{
    // Create socket
    int serverFd;
    if (-1==(serverFd=socket(AF_INET,SOCK_STREAM,0)))
    {
        perror("create listen socket error\n");
        return -1;
    }

    // bind ip  and port
    struct sockaddr_in serverAddr;
    memset(&serverAddr,0,sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    if (-1 == bind(serverFd, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr)))
    {
        perror("bind error\n");
        return -1;
    }

    // listen socket
    if (-1==listen(serverFd, 5))
    {
        perror("listen error\n");
        return -1;
    }

    return serverFd;
}

void *handle_clnt(void *arg)
{
    ClientSocketInfo clntSocketInfo = *(( ClientSocketInfo*)arg);
    int str_len = 0, i;
    char send_buf[BUF_SIZE];
    char recv_buf[BUF_SIZE];
    // int  sendnum = sprintf(send_buf,"hello,the guest from %s\n",inet_ntoa(clnt_adr.sin_addr));
    // if ( 0 >send(connectd,send_buf,sendnum,0))
    // {
    //     perror("send error\n");
    //     close(connectd);
    //     continue;
    // }
    int recvnum;
    if (0>(recvnum = recv(clntSocketInfo.clientfd,recv_buf,sizeof(recv_buf),0)))
    {
        perror("recv error\n");
        close(clntSocketInfo.clientfd);
        return NULL;
    }
    recv_buf[recvnum]='\0';
    printf ("客户端fd:%d \n", clntSocketInfo.clientfd);
    printf ("客户端port:%d \n", clntSocketInfo.clntAddr->sin_port);
    printf ("连接客户端IP: %s \n", inet_ntoa(clntSocketInfo.clntAddr->sin_addr)); //客户端连接的ip地址
    printf ("客户端发来的消息: %s",recv_buf);
    printf ("结束.\n");
    close(clntSocketInfo.clientfd);

    return NULL;
}

void AcceptByMultiThread(int serverfd, pthread_t pid)
{
    sockaddr_in clnt_adr;
    socklen_t clnt_addr_size;
    int clntfd;
    while (1)
    {
        
        if (EISCONN == (clntfd = accept(serverfd, (struct sockaddr*)&clnt_adr, &clnt_addr_size)))
        {
            perror("create connect socket error\n");
            continue;
        }
        ClientSocketInfo clntSocketInfo;
        clntSocketInfo.clientfd = clntfd;
        clntSocketInfo.clntAddr = &clnt_adr;
        pthread_create(&pid, NULL, handle_clnt, (void *)&clntSocketInfo); 
        pthread_detach(pid);
    }
}
