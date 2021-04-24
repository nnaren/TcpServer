#include"socket_helper.h"
#include<sys/select.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>

//函数：更新maxfd
int UpdateMaxfd(fd_set fds, int maxfd) {
    int i;
    int new_maxfd = 0;
    for (i = 0; i <= maxfd; i++) {
        if (FD_ISSET(i, &fds) && i > new_maxfd) {
            new_maxfd = i;
        }
    }
    return new_maxfd;
}
#define SELECT_TIMEOUT 3

void AcceptBySelect(int serverfd)
{

    char buffer[BUF_SIZE] = {0};
    struct timeval timeout = {SELECT_TIMEOUT,0}; //select等待5秒，5秒轮询，要非阻塞就置0

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int new_client_fd;
    int recv_num;
    struct fd_set rfds,rfds_back;
    FD_ZERO(&rfds);
    FD_ZERO(&rfds_back);
    FD_SET(serverfd, &rfds_back);

    int fdIndex;
    int maxfd = serverfd;
    while(1) 
    { 
        
        // 注意select之后readfds和timeout的值都会被修改，因此每次都进行重置
        rfds = rfds_back;

        timeout.tv_sec = SELECT_TIMEOUT;
        timeout.tv_usec = 0;

        maxfd = UpdateMaxfd(rfds, maxfd);
        switch(select(maxfd+1, &rfds, NULL, NULL, &timeout)) //select使用 
        { 
            case -1:
                perror("select failed");
                exit(EXIT_FAILURE);
            case 0:
                printf("no socket ready for read within %d secs\n", SELECT_TIMEOUT);
                continue; //再次轮询 
            default: 
                for (fdIndex = 0; fdIndex <= maxfd; fdIndex++) 
                {
                    if (!FD_ISSET(fdIndex, &rfds)) 
                    {
                        continue;
                    }
                    //可读的socket
                    if ( fdIndex == serverfd) {
                        //当前是server的socket，不进行读写而是accept新连接
                        new_client_fd = accept(serverfd, (struct sockaddr *) &client_addr, &client_addr_len);
                        if (new_client_fd == -1) {
                            perror("accept failed");
                            exit(EXIT_FAILURE);
                        }
                        // if (!inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip_str, sizeof(client_ip_str))) {
                        //     perror("inet_ntop failed");
                        //     exit(EXIT_FAILURE);
                        // }
                        printf("accept a client from: %s\n", inet_ntoa(client_addr.sin_addr));
                        //设置new_sock为non-blocking
                        SetSockNonBlock(new_client_fd);
                        //把new_sock添加到select的侦听中
                        if (new_client_fd > maxfd) {
                            maxfd = new_client_fd;
                        }
                        FD_SET(new_client_fd, &rfds_back);
                    } 
                    else 
                    {
                        //当前是client连接的socket，可以写(read from client)
                        memset(buffer, 0, sizeof(buffer));
                        if ( (recv_num = recv(fdIndex, buffer, sizeof(buffer), 0)) == -1 ) {
                            perror("recv failed");
                            exit(EXIT_FAILURE);
                        }
                        printf("recved from new_sock=%d : %s(%d length string)\n", fdIndex, buffer, recv_num);
                        //立即将收到的内容写回去，并关闭连接
                        if ( send(fdIndex, buffer, recv_num, 0) == -1 ) {
                            perror("send failed");
                            exit(EXIT_FAILURE);
                        }
                        printf("send to new_sock=%d done\n", fdIndex);
                        if ( close(fdIndex) == -1 ) {
                            perror("close failed");
                            exit(EXIT_FAILURE);
                        }
                        printf("close new_sock=%d done\n", fdIndex);
                        //将当前的socket从select的侦听中移除
                        FD_CLR(fdIndex, &rfds_back);
                    }
                }
        }// end switch 
    }//end while 

}