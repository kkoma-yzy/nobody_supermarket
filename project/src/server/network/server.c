/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月04日 星期日 18时55分25秒
 ************************************************************************/

#include<stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include "server.h"
#include <sys/epoll.h>
#include <string.h>
#include "db.h"

struct Cmd cmdArr[] = 
{
    {"register", salerRegister},
    {"login", salerLogin},
    {"insert", salerInsertGoods},
    {"delete", salerDeleteGoods},
    {"update", salerUpdateGoodsPrice},
    {"select", salerSelectGoods},
    {"product", sendGoodsToCustomer},
    {"price", calPrice}
};

/*void dataAnalysis(char *pBuf, char *argv[], int *argc)
{
    if(NULL == pBuf || NULL == argv || NULL == argc)
    {
        return;
    }
}*/


/*
 * 函数名：initServer
 * 函数功能：初始化服务器
 * 函数形参1:unsigned short port 端口号
 * 函数形参2：char *ip IP地址
 * 返回值：int
 *         失败：-1
 *         成功：监听套接字文件描述符
 */
int initServer(unsigned short port, char *ip)
{
    int servFd = -1;
    int ret = 0;
    struct sockaddr_in servAddr = {0};
    

    //创建套接字
    servFd = socket(PF_INET, SOCK_STREAM, 0);
    if(servFd < 0)
    {
        perror("socket error!");
        return SERVER_OK_ERR;
    }
    printf("socket ok!\n");

    //绑定地址信息
    servAddr.sin_family = PF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = inet_addr(ip);
    ret = bind(servFd, (struct sockaddr *)&servAddr, sizeof(servAddr));
    if(ret < 0)
    {
        perror("bind error!");
        close(servFd);
        return SERVER_OK_ERR;
    }
    printf("bind ok!\n");

    //创建监听队列
    ret = listen(servFd, 10);
    if(ret < 0)
    {
        perror("listen error!");
        close(servFd);
        return SERVER_OK_ERR;
    }
    printf("listening....\n");
    return servFd;
}



/*
 *函数名：acceptClient
 *函数功能：和客户端建立连接并通信
 *函数参数：int listenFd 监听套接字文件描述符
 *返回值：int 
          失败：-1
          成功：0
 */

int acceptClient(int listenFd)
{
    int connFd = -1;
    int ret = 0;
    int epfd = -1;
    int nfds = 0;
    struct epoll_event ev = {0};
    struct epoll_event events[EVENTS_MAX_SIZE];
    char msgBuf[BUF_SIZE] = {0};

    if(listenFd < 0)
    {
        return SERVER_OK_ERR;
    }

    //创建epoll对象
    epfd = epoll_create(EVENTS_MAX_SIZE);
    if(epfd < 0)
    {
        perror("epoll_create error!");
        close(listenFd);
        return SERVER_OK_ERR;
    }
    //将listenFd加入到epoll中
    ev.data.fd = listenFd;
    ev.events = EPOLLIN;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenFd, &ev);
    if(ret < 0)
    {
        perror("epoll_ctl_add error!");
        close(listenFd);
        return SERVER_OK_ERR;
    }

    while(1)
    {
        printf("before epoll....\n");
        //等待IO事件
        nfds = epoll_wait(epfd, events, EVENTS_MAX_SIZE, -1);
        if(nfds < 0)
        {
            perror("epoll_wait error!");
            close(listenFd);
            return SERVER_OK_ERR;
        }
        int i;
        for(i = 0; i < nfds; i++)
        {
            if(events[i].data.fd == listenFd)
            {
                printf("waitting for a client....\n");
                //等待客户端并建立连接
                connFd = accept(listenFd, NULL, NULL);
                if(connFd < 0)
                {
                    perror("accept error!");
                    close(listenFd);
                    return SERVER_OK_ERR;
                }
                printf("accept ok!\n");

                ev.data.fd = connFd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connFd, &ev);
            }
            else
            {
                int connFd = events[i].data.fd;
                memset(msgBuf, 0, sizeof(msgBuf));
                ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
                if(ret < 0)
                {
                    perror("recv error!");
                    close(connFd);
                    continue;
                }
                else if(0 == ret)
                {
                    printf("client shutdown...\n");
                    close(connFd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, connFd, &events[i]);
                    continue;
                }
                printf("recv from client:%s\n", msgBuf);
                
                int arrSize = sizeof(cmdArr) / sizeof(cmdArr[0]);
                int i;
                for(i = 0; i < arrSize; i++)
                {
                    if(0 == strcmp(msgBuf, cmdArr[i].cmd))
                    {
                        cmdArr[i].pCmd(connFd);
                    }
                }
            }
        }
    }
    close(listenFd);
    return SERVER_OK;
}
