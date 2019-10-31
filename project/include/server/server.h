/*************************************************************************
	> File Name: server.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月04日 星期日 19时18分49秒
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H

#define EVENTS_MAX_SIZE 100
#define BUF_SIZE  1024


enum SERVER_OK_OP
{
    SERVER_OK_ERR = -1,
    SERVER_OK
};

typedef int (*pFunc)(int);
struct Cmd
{
    char cmd[20];
    pFunc pCmd;
};

int initServer(unsigned short port, char *ip);
int acceptClient(int listenFd);
#endif
