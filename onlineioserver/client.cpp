/*===============================================================
 *   Copyright (C) 2018 All rights reserved.
 *   
 *   文件名称：client.cpp
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
#include <unistd.h>
#include <sqlite3.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define DATABASE "my.db"
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
#define SIZE 100
#include <iostream>
#include <string>
using namespace std;
typedef struct
{
	int type;
	char name[20];
	char data[256];   


}MSG;
class MYtcpsocket
{
	public:
		int connect_server();
		void do_help();
		void do_list(char *order,MSG *msg);
		void do_listlocal();
		void do_get(char *order,MSG *msg);
		void do_put(char *order,MSG *msg);
		void do_register(int socketfd, MSG *msg) ;
		int do_login(int socketfd, MSG *msg) ; 
		void do_changepass(int socketfd , MSG *msg);
} ;
int MYtcpsocket::connect_server()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in srv_addr;
	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family 		= AF_INET;
	srv_addr.sin_port 			= htons(7777);
	srv_addr.sin_addr.s_addr 	= inet_addr("127.0.0.1");
	if (0 > connect(sockfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr)))
	{
		perror("connect");
		return -1;
	}
	return sockfd;
}
void MYtcpsocket::do_help()
{
	cout<<"help  ---------------- 帮助手册"<<endl;
	cout<<"list  ---------------- 列出可下载文件"<<endl;
	cout<<"get <filename>  ------ 下载文件"<<endl;
	cout<<"put <filename>  ------ 上传文件"<<endl;
}
void MYtcpsocket::do_list(char *order,MSG *msg)
{

	int sockfd = connect_server();
	if (sockfd < 0)
	{
		fprintf(stderr, "连接失败!\n");
		return;
	}
	send(sockfd, order, SIZE, 0);
	char buf[128];
	write(sockfd,msg->name,20);
	read(sockfd,buf,sizeof buf);
	printf("%s",buf);

	close(sockfd);
}
void MYtcpsocket::do_listlocal()
{
	system("ls");
}
void MYtcpsocket::do_get(char *order,MSG *msg)
{
	int sockfd = connect_server();

	char *filename = order+4;
	while (*filename == ' ') filename++;

	int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (fd < 0)
	{
		cout<<"下载失败"<<endl;
		return;
	}

	if (sockfd < 0)
	{
		fprintf(stderr, "连接失败!\n");
		close(fd);
		unlink(filename); //用于删除文件
		return;
	}

	send(sockfd, order, SIZE, 0);  //把请求发送给服务器
	char buf[100];
	int ret;
	recv(sockfd, buf, 2, 0);
	if (strncmp(buf, "N", 1) == 0)
	{
		cout<<"下载失败"<<endl;
		close(fd);
		unlink(filename); //用于删除文件
		close(sockfd);
		return;
	}
	while (1)
	{
		ret = read(sockfd, buf, sizeof(buf));
		if (ret <= 0)
			break;
		write(fd, buf, ret);
	}

	close(fd);
	close(sockfd);
}
void MYtcpsocket::do_put(char *order,MSG *msg)
{
	char *filename = order+4;
	while (*filename == ' ') filename++;
	int fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "上传失败\n");
		return;
	}

	int sockfd = connect_server();
	if (sockfd < 0)
	{
		fprintf(stderr, "连接失败!\n");
		close(fd);
		return;
	}
	char sd[128]={0};
	send(sockfd, order, SIZE, 0);
	write(sockfd,msg->name,20);
	read(sockfd,sd,128);
	printf("%s",sd);
	char buf[100];
	int ret;
	recv(sockfd, buf, 2, 0);
	if (strncmp(buf, "N", 1) == 0)
	{
		fprintf(stderr, "上传失败!\n");
		close(fd);
		close(sockfd);
		return;
	}
	while (1)
	{
		ret = read(fd, buf, sizeof(buf));
		if (ret <= 0)
			break;
		write(sockfd, buf, ret);
	}
	close(fd);
	close(sockfd);
}
void MYtcpsocket::do_register(int socketfd, MSG *msg) 
{
	msg->type = 1;
	cout<<"input name : "<<endl;
	cin>>msg->name;
	cout<<"input password : "<<endl;
	cin>>msg->data;
	send(socketfd, msg, sizeof(MSG), 0);
	recv(socketfd, msg, sizeof(MSG), 0);
	cout<<"register : "<<msg->data<<endl;
	return;


}
int MYtcpsocket::do_login(int socketfd, MSG *msg) 
{
	msg->type = 2;
	cout<<"input name : "<<endl;
	cin>>msg->name;
	cout<<"input password : "<<endl;
	cin>>msg->data;
	send(socketfd, msg, sizeof(MSG), 0);
	recv(socketfd, msg, sizeof(MSG), 0);
	if (strncmp(msg->data, "OK", 3) == 0)     
	{
		cout<<"login : OK"<<endl;
		return 1;                            


	}       
	return 0;                                
}
void MYtcpsocket::do_changepass(int socketfd , MSG *msg)
{
	msg->type = 3;
	char buf[20]={0};
	cout<<"input name : "<<endl;
	cin>>msg->name;
	cout<<"input password : "<<endl;
	cin>>msg->data;
	cout<<"input newpassword : "<<endl;
	cin>>buf;
	send(socketfd, msg, sizeof(MSG), 0);
	write(socketfd,buf,20);
	recv(socketfd, msg, sizeof(MSG), 0);
	cout<<" changepass: "<<msg->data<<endl;
	return;

}
int main(int argc, char *argv[])
{
	MYtcpsocket q;
	char order[SIZE];
	int socketfd ;
	MSG msg;
	if ((socketfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("fail to socket");


	}
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(8888);    
	if (connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("fail to connect");
		exit(-1);
	}
	int n;
	while (1) 
	{
loop:	cout<<"************************************"<<endl;
	cout<<"*********** 1:register *************"<<endl;  
	cout<<"*********** 2: login   *************"<<endl;  
	cout<<"*********** 3:changepass ***********"<<endl;
	cout<<"please choose : "<<endl;
	if((cin>>n)<=0)
	{
		perror("scanf");
		exit(-1);
	}
	switch (n)
	{
		case 1 :
			q.do_register(socketfd, &msg);
			break;
		case 2 :

			if(1 == q.do_login(socketfd,&msg))
			{
				goto next;
			}
			break;
		case 3 :
			q.do_changepass(socketfd,&msg);
			break;
	}

	}
next: 	

	cout<<"------------登录成功-----------"<<endl;
	cout<<"-------------1.list------------"<<endl;
	cout<<"-------------2.get<filename>---"<<endl;
	cout<<"-------------3.put<filename>---"<<endl;
	cout<<"-------------4.listlocal-------"<<endl;
	cout<<"-------------5.quit--------退出"<<endl;
	cout<<"-------------6.relogin-重新登录"<<endl;
	cout<<"-------------7.help------------"<<endl;

	memset(order,0,SIZE);
	fgets(order, SIZE, stdin);  //'\n'
	order[strlen(order)-1] = '\0';  //把order末尾的'\n'去掉
	while(1)
	{
		//cout<<endl;
		cout<<"账户 "<<msg.name<<endl;
		//cout<<endl;
		cout<<"请输入指令"<<endl;
		memset(order, 0, SIZE);
		fgets(order, SIZE, stdin);  //'\n'
		order[strlen(order)-1] = '\0';  //把order末尾的'\n'去掉

		if (strncmp(order, "help", 4) == 0)
		{
			q.do_help();
			cout<<endl;
		        cout<<"以上为help内容"<<endl;
		}
		else if (strncmp(order, "get ", 4) == 0)
		{
			q.do_get(order,&msg);
			cout<<endl;
		}
		else if (strncmp(order, "put ", 4) == 0)
		{
			q.do_put(order,&msg);
			cout<<endl;
		}
		else if (strncmp(order, "listlocal", 9) == 0)
		{
			q.do_listlocal();
			cout<<endl;
		        cout<<"以上是当前文件夹本地资源"<<endl;
		}
		else if (strncmp(order, "list", 4) == 0)
		{
			q.do_list(order,&msg);
			cout<<endl;
		        cout<<"以上是上传的云资源"<<endl;
		}
		else if (strncmp(order, "quit", 4) == 0)
		{ 
			cout<<"退出成功"<<endl;
			cout<<endl;
			return 1;
		}
		else if (strncmp(order,"relogin", 7)==0)
		{
			cout<<"重新登录"<<endl;
			cout<<endl;
			goto loop;
		}
		else
			//fprintf(stderr, "Input error, try help!\n");
			cout<<"输入指令有误，请重新指令"<<endl;
	}
	return 0;
}

