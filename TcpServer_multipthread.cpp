#include<sys/socket.h>
#include<stdio.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include"socket_helper.h"


#define NUM_PTHREAD 5


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

