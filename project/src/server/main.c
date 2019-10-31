/*************************************************************************
	> File Name: main.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月04日 星期日 18时56分45秒
 ************************************************************************/

#include<stdio.h>
#include "server.h"
#include "db.h"

#define PORT 9999
#define IP "127.0.0.1"

int main()
{
    int listenFd = -1;
    int ret = 0;

    ret = initDb();
    if(ret < 0)
    {
        return -1;
    }
    printf("Init db ok!\n");

    listenFd = initServer(PORT, IP);
    if(listenFd < 0)
    {
        printf("initServer error!");
        return -1;
    }
    printf("initServer ok!\n");

    ret = acceptClient(listenFd);
    if(ret < 0)
    {
        printf("acceptClient error!");
        return -1;
    }

    return 0;
}
