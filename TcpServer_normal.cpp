#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string>
#include<errno.h>
#include <unistd.h>

#include"socket_helper.h"

int ReadyforTcpServer(unsigned short port, int blockFlag)
{
    // Create socket
    int serverfd;
    if (-1==(serverfd=socket(AF_INET,SOCK_STREAM,0)))
    {
        perror("create listen socket error\n");
        return -1;
    }
    if(1 == blockFlag)
    {
        SetSockNonBlock(serverfd);
    }
    
    // bind ip  and port
    struct sockaddr_in serverAddr;
    memset(&serverAddr,0,sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    if (-1 == bind(serverfd, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr)))
    {
        perror("bind error\n");
        return -1;
    }

    // listen socket
    if (-1==listen(serverfd, 5))
    {
        perror("listen error\n");
        return -1;
    }

    return serverfd;
}

void AcceptNormal(int serverfd)
{
    sockaddr_in clnt_adr;
    socklen_t clnt_addr_size;
    int clntfd, sendnum, recvnum;
    char send_buf[BUF_SIZE];
    char recv_buf[BUF_SIZE];
    memset(send_buf,0,BUF_SIZE);
    memset(recv_buf,0,BUF_SIZE);
    while (1)
    {
        if (EISCONN==(clntfd=accept(serverfd, (struct sockaddr*)&clnt_adr, &clnt_addr_size)))
        {
            perror("create connect socket error\n");
            continue;
        }
    
        sendnum = sprintf(send_buf,"hello,the guest from %s\n",inet_ntoa(clnt_adr.sin_addr));
        if ( 0 > send(clntfd, send_buf, sendnum, 0))
        {
            perror("send error\n");
            close(clntfd);
            continue;
        }
    
        if (0>(recvnum = recv(clntfd, recv_buf, sizeof(recv_buf),0)))
        {
            perror("recv error\n");
            close(clntfd);
            continue;
        }
        recv_buf[recvnum]='\0';
    
        printf ("客户端发来的消息: %s\n", recv_buf);
    
        if (0==strcmp(recv_buf,"quit"))
        {
            perror("the client break the server process\n");
            close(clntfd);
            break;
        }
    
        sendnum = sprintf(send_buf,"%s\n",recv_buf);
    
        send(clntfd,send_buf,sendnum,0);
    
        close(clntfd);
    }
}