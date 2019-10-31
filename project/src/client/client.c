/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月04日 星期日 19时20分02秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 1024
#define PORT 9999
#define IP "127.0.0.1"

char msgBuf[BUF_SIZE] = {0};

int sendData(int cliFd, const char *data)
{
    if(cliFd < 0 || NULL == data)
    {
        return -1;
    }
    memset(msgBuf, 0, sizeof(msgBuf));
    strcpy(msgBuf, data);
    int ret = send(cliFd, msgBuf, sizeof(msgBuf), 0);
    if(ret < 0)
    {
        perror("send error!");
        close(cliFd);
        return -1;
    }
    printf("send ok!\n");
}

int main()
{
    int cliFd = -1;
    int ret = 0;
    struct sockaddr_in servAddr = {0};
    char choice_mode;
    char choice_login;
    char cmd[20] = {0};

    //创建套接字
    cliFd = socket(PF_INET, SOCK_STREAM, 0);
    if(cliFd < 0)
    {
        perror("socket error!");
        return -1;
    }
    printf("socket ok!\n");

    //连接服务器
    servAddr.sin_family = PF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = inet_addr(IP);
    ret = connect(cliFd, (struct sockaddr *)&servAddr, sizeof(servAddr));
    if(ret < 0)
    {
        perror("connect error!");
        close(cliFd);
        return -1;
    }
    printf("connect ok!\n");

    while(1)
    {
    loop1:
        printf("####################################\n");
        printf("#*************WELCOME**************#\n");
        printf("#*****please choice login mode*****#\n");
        printf("#*****1.saler   ***  2.customer****#\n");
        printf("#**********************************#\n");
        printf("####################################\n");
        choice_mode = getchar();
        printf("choice_mode = %c\n", choice_mode);
        //处理垃圾字符
        getchar();
        switch(choice_mode)
        {
            case '1':
            loop:
                printf("1.register  2.login\n");
                choice_login = getchar();
                getchar();
                switch(choice_login)
                {
                    case '1':
                    {
                    reg:
                        printf("register....\n");
                        ret = sendData(cliFd, "register");
                        if(ret < 0)
                        {
                            return -1;
                        }
                        while(1)
                        {
                            ret = recv(cliFd, msgBuf, sizeof(msgBuf), 0);
                            if(ret > 0)
                            {
                                if(0 == strcmp(msgBuf, "success"))
                                {
                                    break;
                                }
                                else if(0 == strcmp(msgBuf, "error"))
                                {
                                    printf("register error!\n");
                                    printf("please register again!\n");
                                    goto reg;
                                }
                                printf("%s\n", msgBuf);
                                char buf[1024] = {0};
                                gets(buf);
                                sendData(cliFd, buf);
                            }
                        }
                        printf("register sucess!\n");
                        goto loop;
                    }
                    case '2':
                    {
                    log:
                        printf("login...\n");
                        ret = sendData(cliFd, "login");
                        if(ret < 0)
                        {
                            return -1;
                        }
                        while(1)
                        {
                            ret = recv(cliFd, msgBuf, sizeof(msgBuf), 0);
                            if(ret > 0)
                            {
                                if(0 == strcmp(msgBuf, "success"))
                                {
                                    break;
                                }
                                else if(0 == strcmp(msgBuf, "error"))
                                {
                                    printf("login error!\n");
                                    printf("please login again!\n");
                                    goto log;
                                }
                                printf("%s\n", msgBuf);
                                char buf[1024] = {0};
                                gets(buf);
                                sendData(cliFd, buf);
                            }
                        }
                        printf("login sucess!\n");
                        while(1)
                        {
                            printf("please input cmd：insert/delete/update/select\n");
                            memset(cmd, 0, sizeof(cmd));
                            gets(cmd);
                            if(0 == strcmp(cmd, "quit"))
                            {
                                goto loop1;
                            }
                            ret = sendData(cliFd, cmd);
                            if(ret < 0)
                            {
                                return -1;
                            }
                            int n = 0;
                            while(1)
                            {
                                ret = recv(cliFd, msgBuf, sizeof(msgBuf), 0);
                                if(ret > 0)
                                {
                                    if(0 == strcmp(msgBuf, "success"))
                                    {
                                        break;
                                    }
                                    else if(0 == strcmp(msgBuf, "error"))
                                    {
                                        printf("error!\n");
                                        printf("see you next time!\n");
                                        return -1;
                                    }
                                    //如果是select继续接收
                                    if(0 == strcmp(cmd, "select") && 1 == n)
                                    {
                                        printf("%s", msgBuf);
                                        continue;
                                    }
                                    printf("%s\n", msgBuf);
                                    char buf[1024] = {0};
                                    gets(buf);
                                    sendData(cliFd, buf);
                                    //n用来判断select来过一次
                                    n++;
                                }
                            }
                            printf("success!\n");
                        }
                        break;
                    }
                }
            case '2':
            {
                printf("******product list******\n");
                ret = sendData(cliFd, "product");
                if(ret < 0)
                {
                    return -1;
                }
                //用来记录商品的种类数
                int count = 0;
                while(1)
                {
                    ret = recv(cliFd, msgBuf, sizeof(msgBuf), 0);
                    if(ret > 0)
                    {
                        if(0 == strcmp(msgBuf, "success"))
                        {
                            break;
                        }
                        else if(0 == strcmp(msgBuf, "error"))
                        {
                            printf("error!\n");
                            printf("see you next time!\n");
                            return -1;
                        }
                        count++;
                        printf("%s", msgBuf);
                    }
                }
                //选择要购买的商品
                printf("***Please input goods that you want to buy***\n");
                printf("ok means over\n");
                //申请空间，用来保存选中的商品
                //商品名字
                char (*p)[20] = (char (*)[20])malloc((count+1) * 20);
                if(NULL == p)
                {
                    printf("malloc error!\n");
                    return -1;
                }
                //商品价格
                char (*q)[20] = (char (*)[20])malloc(count*20);
                if(NULL == q)
                {
                    printf("malloc error!\n");
                    return -1;
                }
                memset(p, 0, 20 * (count+1));
                memset(q, 0, 20 * count);
                int i = 0;
                while(1)
                {
                    printf("goodsName:");
                    gets(*(p+i));
                    if(0 == strcmp(p[i], "quit"))
                    {
                        goto loop1;
                    }
                    else if(0 == strcmp(p[i], "ok"))
                    {
                        break;
                    }
                    printf("goodsCount:");
                    gets(*(q+i));
                    i++;
                }
                printf("*****Please make sure your goods*****\n");
                int j = 0;
                while(j < i)
                {
                    printf("%s\t%-15s\n", p[j], q[j]);
                    j++;
                }
                printf("Please make sure, input ok!");
                memset(msgBuf, 0, sizeof(msgBuf));
                gets(msgBuf);
                if(0 == strcmp(msgBuf, "ok"))
                {
                    sendData(cliFd, "price");
                    sleep(1);
                    j = 0;
                    while(j < i)
                    {
                        //将选中的商品信息发送给服务器
                        sendData(cliFd, p[j]);
                        sendData(cliFd, q[j]);
                        j++;
                    }
                    sendData(cliFd, "over");
                    //接收服务器计算后的总价格
                    int totalPrice = 0;
                    int price = 0;
                    recv(cliFd, &totalPrice, sizeof(int), 0);
                    pay:
                    printf("PLease pay for %d yuan\n", totalPrice);
                    scanf("%d", &price);
                    //处理垃圾字符'\n'
                    getchar();
                    if(price == totalPrice)
                    {
                        printf("###########################\n");
                        printf("#****See you next time****#\n");
                        printf("#********Bye Bye!*********#\n");
                        printf("###########################\n");
                        printf("\n");
                        printf("\n");
                        break;
                    }
                    else
                    {
                        printf("****PLease pay for again****\n");
                        goto pay;
                    }
                }
            }
            break;
        }
    }
    close(cliFd);
    return 0;
}
