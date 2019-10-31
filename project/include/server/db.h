/*************************************************************************
	> File Name: db.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月04日 星期日 19时18分49秒
 ************************************************************************/

#ifndef _DB_H
#define _DB_H

int initDb();
int salerRegister(int connFd);
int salerLogin(int connFd);
int salerInsertGoods(int connFd);
int salerDeleteGoods(int connFd);
int salerUpdateGoodsPrice(int connFd);
int salerSelectGoods(int connFd);
int sendGoodsToCustomer(int connFd);
int calPrice(int connFd);
#endif
