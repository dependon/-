/*===============================================================
 *   Copyright (C) 2018 All rights reserved.
 *   
 *   文件名称：ioserver.cpp
 *   创 建 者：刘明航
 *   创建日期：2018年08月16日
 *   描    述：
 *
 *   更新日志：
 *
 ================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sqlite3.h>
#define SIZE 100
#include <iostream>
#include <string>
using namespace std;
class ioserver
{
	public:
		void do_list(int connfd,sqlite3 *db);
		void do_get(int connfd, char *order);
		void do_put(int connfd, char *order,sqlite3 *db);
};
void ioserver::do_list(int connfd,sqlite3 *db)
{
	char sqlstr[128];
	char name[20]={0};
	char *errmsg;
	char **resultp;
	int nrow,ncolumn;
	int i=0,j=0,index;
	char buf[128]={0};
	read(connfd,name,sizeof name);
	sprintf(sqlstr,"select * from %s;",name);
	cout<<sqlstr<<endl;
	if(sqlite3_get_table(db,sqlstr,&resultp,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
	{
		cout<<"error : "<<errmsg<<endl;
		cout<<"no ok"<<endl;
		exit(-1);
	}
	for(i=1;i<=nrow;i++)
	{
		strcat(strcat(buf,resultp[i])," ");
		cout<<"ok"<<endl;
	}
	write(connfd,buf,sizeof buf);
}
void ioserver::do_get(int connfd, char *order)
{
	while (*order == ' ') order++;
	char buf[100];
	int ret;
	int fd = open(order, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		send(connfd, "N", 2, 0);
		return;
	}
	else
		send(connfd, "Y", 2, 0);

	while (1)
	{
		ret = read(fd, buf, sizeof(buf));
		if (ret <= 0)
			break;
		write(connfd, buf, ret);
	}
	close(fd);
}
void ioserver::do_put(int connfd, char *order,sqlite3 *db)
{
	while (*order == ' ') order++;
	char sqlstr[128];
	char sd[128]={0};
	char name[20]={0};
	char *errmsg;
	read(connfd,name,20);
	sprintf(sqlstr,"insert into %s values('%s')",name,order);
	if(sqlite3_exec(db,sqlstr,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		sqlite3_free(errmsg);
		sprintf(sd,"no ok\n");
	}
	else
	{
		strncpy(sd,"ok",128);
	}
	write(connfd,sd,128);

	int fd = open(order, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (fd < 0)
	{
		perror("open");
		send(connfd, "N", 2, 0);
		return;
	}
	else
		send(connfd, "Y", 2, 0);

	char buf[100];
	int ret;
	while (1)
	{
		ret = read(connfd, buf, sizeof(buf));
		if (ret <= 0)
			break;
		write(fd, buf, ret);
	}
	close(fd);
}
int main()
{
	ioserver q;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in myaddr;
	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family 		= AF_INET;
	myaddr.sin_port 		= htons(7777);
	myaddr.sin_addr.s_addr 	= inet_addr("127.0.0.1");
	if (0 > bind(sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr)))
	{
		perror("bind");
		return -1;
	}

	if (0 > listen(sockfd, 1024))
	{
		perror("listen");
		return -1;
	}
	sqlite3 *db;
	if (sqlite3_open("my.db", &db) != SQLITE_OK)
	{
		printf("error : %s\n", sqlite3_errmsg(db));
		exit(-1);

	}
	char order[SIZE];
	while (1)
	{
		int connfd = accept(sockfd, NULL, NULL);
		if (connfd < 0)
		{
			perror("accept");
			break;
		}
		memset(order, 0, SIZE);
		recv(connfd, order, SIZE, 0);
		if (strncmp(order, "list", 4) == 0)
			q.do_list(connfd,db);
		else if (strncmp(order, "get", 3) == 0)
			q.do_get(connfd, order+4);
		else if (strncmp(order,"put",3)==0)
			q.do_put(connfd, order+4,db);

		close(connfd);
	}

	return 0;
}	


