#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
 
#define PORT 8900
 
int main(int argc,char** argv)
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;
    int port;
    int listend;
    int connectd;
    int sendnum;
    int opt;
    int recvnum;
    char send_buf[2048];
    char recv_buf[2048];
 
 
    port= PORT;
    memset(send_buf,0,2048);
    memset(recv_buf,0,2048);
    
    opt = SO_REUSEADDR;
      
 
    if (-1==(listend=socket(AF_INET,SOCK_STREAM,0)))
    {
        perror("create listen socket error\n");
        exit(1);
    }
    setsockopt(listend,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
 
 
    memset(&server,0,sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
 
    if (-1==bind(listend,(struct sockaddr *)&server,sizeof(struct sockaddr)))
    {
        perror("bind error\n");
        exit(1);
    }
 
    if (-1==listen(listend,5))
    {
        perror("listen error\n");
        exit(1);
    }
 
    while (1)
    {
        if (-1==(connectd=accept(listend, (struct sockaddr*)&client, &len)))
        {
            perror("create connect socket error\n");
            continue;
        }
 
    
    sendnum = sprintf(send_buf,"hello,the guest from %s\n",inet_ntoa(client.sin_addr));
    if ( 0 >send(connectd,send_buf,sendnum,0))
    {
        perror("send error\n");
        close(connectd);
        continue;
    }
    
 
    if (0>(recvnum = recv(connectd,recv_buf,sizeof(recv_buf),0)))
    {
        perror("recv error\n");
        close(connectd);
        continue;
    }
    recv_buf[recvnum]='\0';
 
    printf ("客户端发来的消息: %s\n",recv_buf);
 
    if (0==strcmp(recv_buf,"quit"))
    {
        perror("the client break the server process\n");
        close(connectd);
        break;
    }
 
    sendnum = sprintf(send_buf,"%s\n",recv_buf);
  
    send(connectd,send_buf,sendnum,0);
  
    close(connectd);
    continue;
    }
    close(listend);
    return 0;
}