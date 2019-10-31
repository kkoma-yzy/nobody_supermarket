/*************************************************************************
	> File Name: db.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月04日 星期日 18时56分16秒
 ************************************************************************/

#include<stdio.h>
#include "common.h"
#include "sqliteinterface.h"

char msgBuf[1024] = {0};


/*
 * 函数名：initDb
 * 函数功能：创建数据库及创建表
 * 函数形参：无
 * 返回值：int
 *         失败：-1
 *         成功：0
 */

#define FILEPATH_MAX (80)
int initDb()
{
    char *path = NULL;
    char dbName[] = "/mysqlite.db";
    path = (char *)malloc(FILEPATH_MAX + 10);
    if(NULL == path)
    {
        return -1;
    }
    memset(path, 0, FILEPATH_MAX+10);
    getcwd(path, FILEPATH_MAX);
    printf("%s\n", path);
    memcpy(path+strlen(path), dbName, strlen(dbName));
    printf("%s\n", path);

    //init db
    if(DB_ERROR_OK != InitConnection())
    {
        printf("InitConnection error!\n");
        return -1;
    }
    //open or create db
    if(DB_ERROR_OK != OpenConnection(path))
    {
        printf("OpenConnection error!\n");
        return -1;
    }
    //check globle db handle
    if(NULL == g_db)
    {
        printf("open or create db error!\n");
        return -1;
    }
    //create table
    char sqlCreateTable1[] = "create table userRegister(usrname text primary key, pwd text not null)";
    ExecSQL(sqlCreateTable1);

    char sqlCreateTable2[] = "create table goodsInfo(name text primary key, price integer not null, count integer not null)";
    ExecSQL(sqlCreateTable2);
}

/*函数名：senddata
 *函数功能：服务器给客户端发送消息
 *形参1：connFd 连接套接字文件描述符
 *形参2：data  要发送的字符串
 *返回值：成功 0
 *        失败 -1
 */

int sendData(int connFd, const char *data)
{
    if(connFd < 0 || NULL == data)
    {
        return -1;
    }
    memset(msgBuf, 0, sizeof(msgBuf));
    strcpy(msgBuf, data);
    int ret = send(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
        perror("send error!");
        close(connFd);
        return -1;
    }
    printf("send ok!\n");
}

/*
 *函数名：salerRegister
 *函数功能：商家注册
 *参数：connFd 连接套接字文件描述符
 *返回值：成功 0
 *        失败 -1
 */
 
 int salerRegister(int connFd)
 { 
    printf("salerRegister...\n");
    char usrname[20] = {0};
    char pwd[20] = {0};
    char msgBuf[1024] = {0};
    int ret = 0;

    //用户名
    ret = sendData(connFd, "please input usrname:");
    if(ret < 0)
    {
        return -1;
    }
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret > 0)
    {
        strcpy(usrname, msgBuf);
    }

    puts(msgBuf);
    //密码
    ret = sendData(connFd, "please input pwd:");
    if(ret < 0)
    {
        return -1;     
    }

    printf("wait pwd...\n");
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
        perror("recv error!");
        return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    strcpy(pwd, msgBuf);
    puts(msgBuf);
    char sqlInsertTable[1024] = {0};
    sprintf(sqlInsertTable, "insert into userRegister (usrname, pwd) values ('%s', '%s')", usrname, pwd);
    if(DB_ERROR_OK != ExecSQL(sqlInsertTable))
    {
       printf("register error!\n");
       sendData(connFd, "error");
       return -1;
    }
    printf("register success!\n");
    ret = sendData(connFd, "success");
    if(ret < 0)
    {
        return -1;     
    }
    return 0;    
 }

/*函数名：salerLogin
 *函数功能：商家登录
 *参数：connFd 连接套接字文件描述符
 *返回值：成功 0
 *        失败 -1
 */
int salerLogin(int connFd)
{
    printf("salerLogin...\n");
    char usrname[20] = {0};
    char pwd[20] = {0};
    char msgBuf[1024] = {0};
    int ret = 0;

    //用户名
    ret = sendData(connFd, "please input usrname:");
    if(ret < 0)
    {
        return -1;
    }
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
        perror("recv error!");
        return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    strcpy(usrname, msgBuf);
    puts(msgBuf);
    //密码
    ret = sendData(connFd, "please input pwd:");
    if(ret < 0)
    {
        return -1;     
    }

    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
       perror("recv error!");
       return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    strcpy(pwd, msgBuf);
    puts(msgBuf);

    //判断是否正确
    char **ppTable = NULL;
    char *pErrMsg = NULL;
    int iRow = 0;
    int iColumn = 0;
    int n = 0;

    char sqlSelectTable[1024] = {0};
    sprintf(sqlSelectTable, "select pwd from userRegister where usrname='%s'", usrname);
    if(DB_ERROR_OK != GetTable(sqlSelectTable, &ppTable, &iRow, &iColumn))
    {
        printf("login error!\n");
        sendData(connFd, "error");
        return -1;
    }
    if(NULL != ppTable)
    {
        int i = 0;
        for(; i < iRow+1; i++)
        {
            int j = 0;
            for(; j < iColumn; j++)
            {
                if(NULL != ppTable[i*iColumn+j])
                {
                    printf("%s  ", ppTable[i * iColumn +j]);
                    n = i*iColumn+j;
                    printf("n = %d\n", n);
                }
            }
            printf("\n");
        }
    }
    printf("ppTable[1] = %s\n", ppTable[1]);
    if(0 == strcmp(pwd, ppTable[1]))
    {
        printf("login success!\n");
        ret = sendData(connFd, "success");
        if(ret < 0)
        {
            return -1;     
        }
    }
    else
    {
        printf("login error!\n");
        sendData(connFd, "error");
        return -1;
    }

    return 0;
}

/*函数名：salerInsertGoods
 *函数功能：商家添加商品信息
 *参数：connFd 连接套接字文件描述符
 *返回值：成功 0
 *        失败 -1
 */
int salerInsertGoods(int connFd)
{
    printf("salerInsertGoods...\n");
    char name[20] = {0};
    int price = 0;
    int count = 0;
    int ret;
    
    //商品名称
    ret = sendData(connFd, "please input goods name:");
    if(ret < 0)
    {
        return -1;
    }
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
       perror("recv error!");
       return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    strcpy(name, msgBuf);
    printf("goods name:%s\n",name);

    //商品价格
    ret = sendData(connFd, "please input goods price:");
    if(ret < 0)
    {
        return -1;
    }
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
       perror("recv error!");
       return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    price = atoi(msgBuf);
    printf("goods price:%d\n", price);

    //商品数量
    ret = sendData(connFd, "please input goods count:");
    if(ret < 0)
    {
        return -1;
    }
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
       perror("recv error!");
       return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    count = atoi(msgBuf);
    printf("goods count:%d\n", count);

    //将商品信息插入数据库
    char sqlInsertTable[1024] = {0};
    sprintf(sqlInsertTable, "insert into goodsInfo (name, price, count) values ('%s', %d, %d)", name, price, count);
    if(DB_ERROR_OK != ExecSQL(sqlInsertTable))
    {
       printf("register error!\n");
       sendData(connFd, "error");
       return -1;
    }
    printf("insert goods success!\n");
    ret = sendData(connFd, "success");
    if(ret < 0)
    {
        return -1;     
    }
    return 0;
}


/*函数名：salerDeleteGoods
 *函数功能：商家根据商品名字删除商品信息
 *参数：connFd 连接套接字文件描述符
 *返回值：成功 0
 *        失败 -1
 */
int salerDeleteGoods(int connFd)
{
    printf("salerDeleteGoods...\n");
    char name[20] = {0};
    int ret;

    //商品名称
    ret = sendData(connFd, "please input goods name:");
    if(ret < 0)
    {
        return -1;
    }
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
       perror("recv error!");
       return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    strcpy(name, msgBuf);

    //根据商品名称删除
    char sqlDeleteTable[1024] = {0};
    sprintf(sqlDeleteTable, "delete from goodsInfo where name='%s'", name);
    if(DB_ERROR_OK != ExecSQL(sqlDeleteTable))
    {
       printf("delete error!\n");
       sendData(connFd, "error");
       return -1;
    }
    printf("delete goods success!\n");
    ret = sendData(connFd, "success");
    if(ret < 0)
    {
        return -1;     
    }
    return 0;
}


/*函数名：salerUpdateGoodsPrice
 *函数功能：商家根据商品名字更改商品价格
 *参数：connFd 连接套接字文件描述符
 *返回值：成功 0
 *        失败 -1
 */
int salerUpdateGoodsPrice(int connFd)
{
    printf("salerUpdateGoodsPrice...\n");
    char name[20] = {0};
    int price = 0;
    int ret = 0;


    //商品名称
    ret = sendData(connFd, "please input goods name:");
    if(ret < 0)
    {
        return -1;
    }
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
       perror("recv error!");
       return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    strcpy(name, msgBuf);

    //商品价格
    ret = sendData(connFd, "please input new goods price:");
    if(ret < 0)
    {
        return -1;
    }
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
       perror("recv error!");
       return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    price = atoi(msgBuf);
    printf("goods price:%d\n", price);
	//商品
    ret = sendData(connFd, "please input new goods price:");
    if(ret < 0)
    {
        return -1;
    }
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
       perror("recv error!");
       return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    price = atoi(msgBuf);
    printf("goods price:%d\n", price);


    //根据商品名称修改商品价格
    char sqlDeleteTable[1024] = {0};
    sprintf(sqlDeleteTable, "update goodsInfo set price=%d where name='%s'", price, name);
    if(DB_ERROR_OK != ExecSQL(sqlDeleteTable))
    {
       printf("update error!\n");
       sendData(connFd, "error");
       return -1;
    }
    printf("update goods price success!\n");
    ret = sendData(connFd, "success");
    if(ret < 0)
    {
        return -1;     
    }
    return 0;

}


/*函数名：salerSelectGoods
 *函数功能：商家查看所有商品信息或某一个商品信息
 *参数：connFd 连接套接字文件描述符
 *返回值：成功 0
 *        失败 -1
 */
int salerSelectGoods(int connFd)
{
    int ret;
    char name[20] = {0};
    char **ppTable = NULL;
    char *pErrMsg = NULL;
    int iRow = 0;
    int iColumn = 0;

    printf("salerSelectGoods...\n");
    //可以根据商品名称查看某个的商品信息，也可以查看所有商品的信息
    ret = sendData(connFd, "please input goods name or all:");
    if(ret < 0)
    {
        return -1;
    }
    ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
        perror("recv error!");
        return -1;
    }
    else if(0 == ret)
    {
        printf("client shutdown...\n");
        return -1;
    }
    if(0 == strcmp(msgBuf, "all"))
    {
        //查看全部商品信息
        char sqlSelectTable[1024] = "select * from goodsInfo";
        if(DB_ERROR_OK != GetTable(sqlSelectTable, &ppTable, &iRow, &iColumn))
        {
            printf("select error!\n");
            sendData(connFd, "error");
            return -1;
        }
        if(NULL != ppTable)
        {
            int i = 0;
            for(; i < iRow+1; i++)
            {
                int j = 0;
                for(; j < iColumn; j++)
                {
                    if(NULL != ppTable[i*iColumn+j])
                    {
                        printf("%s\t", ppTable[i * iColumn +j]);
                        char buf[1024] = {0};
                        sprintf(buf, "%s\t", ppTable[i*iColumn+j]);
                        ret = sendData(connFd, buf);
                        if(ret < 0)
                        {
                            return -1;
                        }
                    }
                }
                sendData(connFd, "\n");
                printf("\n");
            }
            ret = sendData(connFd, "success");
            if(ret < 0)
            {
                return -1;     
            }
        }
        return 0;
    }
    ppTable = NULL;
    pErrMsg = NULL;
    iRow = 0;
    iColumn = 0;
    strcpy(name, msgBuf);
    char sqlSelectTable[1024] = {0};
    sprintf(sqlSelectTable, "select name, price, count from goodsInfo where name='%s'", name);
    if(DB_ERROR_OK != GetTable(sqlSelectTable, &ppTable, &iRow, &iColumn))
    {
        printf("select error!\n");
        sendData(connFd, "error");
        return -1;
    }
    if(NULL != ppTable)
    {
        int i = 0;
        for(; i < iRow+1; i++)
        {
            int j = 0;
            for(; j < iColumn; j++)
            {
                if(NULL != ppTable[i*iColumn+j])
                {
                    printf("%s\t", ppTable[i * iColumn +j]);
                    char buf[1024] = {0};
                    sprintf(buf, "%s\t", ppTable[i*iColumn+j]);
                    ret = sendData(connFd, buf);
                    if(ret < 0)
                    {
                        return -1;
                    }
                }
            }
            sendData(connFd, "\n");
            printf("\n");
        }
        ret = sendData(connFd, "success");
        if(ret < 0)
        {
            return -1;
        }
    }
    return 0;
}


/*函数名：sendGoodsToCustomer
 *函数功能:发送所有商品信息给客户端
 *参数：connFd 连接套接字文件描述符
 *返回值：成功 0
 *        失败 -1
 */
int sendGoodsToCustomer(int connFd)
{
    int ret;
    char **ppTable = NULL;
    char *pErrMsg = NULL;
    int iRow = 0;
    int iColumn = 0;
    printf("sendGoodsToCustomer...\n");
    char sqlSelectTable[1024] = "select * from goodsInfo";
    if(DB_ERROR_OK != GetTable(sqlSelectTable, &ppTable, &iRow, &iColumn))
    {
        printf("select error!\n");
        sendData(connFd, "error");
        return -1;
    }
    if(NULL != ppTable)
    {
        int i = 0;
        for(; i < iRow+1; i++)
        {
            int j = 0;
            for(; j < iColumn; j++)
            {
                if(NULL != ppTable[i*iColumn+j])
                {
                    printf("%s\t", ppTable[i * iColumn +j]);
                    char buf[1024] = {0};
                    sprintf(buf, "%s\t", ppTable[i*iColumn+j]);
                    ret = sendData(connFd, buf);
                    if(ret < 0)
                    {
                        return -1;
                    }
                }
            }
            sendData(connFd, "\n");
            printf("\n");
        }
        ret = sendData(connFd, "success");
        if(ret < 0)
        {
            return -1;     
        }
    }
    return 0;

}



/*函数名：salerInsertGoods
 *函数功能：根据客户选中的商品计算总价
 *参数：connFd 连接套接字文件描述符
 *返回值：成功 0
 *        失败 -1
 */
int calPrice(int connFd)
{
    printf("calPrice....\n");
    int ret = 0;
    char name[20] = {0};
    int count = 0;
    int totalPrice = 0;
    char **ppTable = NULL;
    char *pErrMsg = NULL;
    int iRow = 0;
    int iColumn = 0;

    while(1)
    {
        //接收商品名字
        ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
        if(ret < 0)
        {
            perror("recv error!");
            return -1;
        }
        else if(0 == ret)
        {
            printf("client shutdown...\n");
            return -1;
        }
        if(0 == strcmp(msgBuf, "over"))
        {
            break;
        }
        strcpy(name, msgBuf);
        //接收商品数量
        ret = recv(connFd, msgBuf, sizeof(msgBuf), 0);
        if(ret < 0)
        {
            perror("recv error!");
            return -1;
        }
        else if(0 == ret)
        {
            printf("client shutdown...\n");
            return -1;
        }
        count = atoi(msgBuf); 
        //按照商品名字查找商品价格
        char sqlSelectTable[1024] = {0};
        sprintf(sqlSelectTable, "select price, count from goodsInfo where name='%s'", name);
        if(DB_ERROR_OK != GetTable(sqlSelectTable, &ppTable, &iRow, &iColumn))
        {
            printf("select error!\n");
            sendData(connFd, "error");
            return -1;
        }
        printf("ppTable[1] = %s\n", ppTable[1]);
        totalPrice += atoi(ppTable[2])*count;
        int n = atoi(ppTable[3]);
        //修改商品数量
        char sqlDeleteTable[1024] = {0};
        sprintf(sqlDeleteTable, "update goodsInfo set count=%d where name='%s'", n-count, name);
        if(DB_ERROR_OK != ExecSQL(sqlDeleteTable))
        {
            printf("update error!\n");
            sendData(connFd, "error");
            return -1;
        }
        printf("update goods count success!\n");
    }

    //将计算好的价格发给客户端
    send(connFd, &totalPrice, sizeof(totalPrice), 0);
    return 0;
}
