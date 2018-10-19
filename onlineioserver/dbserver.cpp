#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define DATABASE "my.db"
#include <iostream>
#include <string>
using namespace std;
typedef struct
{
	int type;
	char name[20];
	char data[256];   


}MSG;

class dbserver
{
	public:
		void do_register(int connectfd, MSG *msg, sqlite3 *db);
		void do_login(int connectfd, MSG *msg, sqlite3 *db);
		void do_changepass(int connectfd, MSG *msg, sqlite3 *db) ;
		void usr();
};
void dbserver::do_register(int connectfd, MSG *msg, sqlite3 *db)
{
	char sqlstr[128];
	char buf[100]={0};
	char *errmsg;
	sprintf(sqlstr, "insert OR IGNORE into usr values ('%s', '%s')", msg->name, msg->data);
	printf("%s\n", sqlstr);
	if (sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != SQLITE_OK) 
	{
		sqlite3_free(errmsg);
		sprintf(msg->data, "user %s already exist!!!", msg->name);
	}
	else
	{
		strncpy(msg->data, "OK", 256);

	}
	bzero(sqlstr, 128);
	sprintf(sqlstr, "create table %s(filename char);", msg->name);
	if(sqlite3_exec(db, sqlstr, NULL,  NULL, &errmsg) !=  SQLITE_OK)
	{
		printf("error :  %s\n",  errmsg);
		exit(-1);
	}
	strcpy(buf, "ok");
	send(connectfd, msg, sizeof(MSG), 0);

	return;
}
void dbserver::do_login(int connectfd, MSG *msg, sqlite3 *db)
{
	char sqlstr[128];
	char *errmsg, **result;
	int nrow, ncolumn;
	sprintf(sqlstr, "select * from usr where name = '%s' and data = '%s'", msg->name, msg->data);
	if (sqlite3_get_table(db, sqlstr, &result, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error : %s\n", errmsg);
		sqlite3_free(errmsg);


	}
	if (nrow == 0)        
	{
		strncpy(msg->data, "name or password is wrong!!!", 256);

	}
	else                  
	{
		strncpy(msg->data, "OK", 256);


	}
	send(connectfd, msg, sizeof(MSG), 0);
	sqlite3_free_table(result);
	return;
}
void dbserver::do_changepass(int connectfd, MSG *msg, sqlite3 *db)
{
	char sqlstr[128];
	char *errmsg, **result;
	int nrow, ncolumn;
	char buf[20]={0};
	read(connectfd,buf,20);
	sprintf(sqlstr,"update usr set data='%s' where name='%s' and data='%s';",buf,msg->name,msg->data);
	if (sqlite3_get_table(db, sqlstr, &result, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		strncpy(msg->data,"更改失败",256);
	}
	else
	{
		strncpy(msg->data, "OK", 256);

	}
	send(connectfd, msg, sizeof(MSG), 0);
	sqlite3_free_table(result);
	return;
} 
void dbserver::usr()
{
	char sql1[128];
	char buf1[100]={0};
	char *errmsg1;
	system("touch my.db");
	sqlite3 *db1;
	if(sqlite3_open(DATABASE,&db1)!= SQLITE_OK)
	{
		cout<<"create error1"<<endl;
	}
	sprintf(sql1,"create table usr (name char PRIMARY KEY,data char);");
	if(sqlite3_exec(db1, sql1, NULL, NULL, &errmsg1)!=SQLITE_OK)
	{
		cout<<"usr数据库已经存在，请放心使用"<<endl;
		sqlite3_free(errmsg1);
	}
        else
	{
		cout<<"my.db usr create ok!!!"<<endl;
	}
	bzero(sql1,128);
	sqlite3_close(db1);
}
void do_client(int connectfd, sqlite3 *db)
{
	MSG msg;
	dbserver q;
	while (recv(connectfd, &msg, sizeof(msg), 0) > 0)  
	{
		switch ( msg.type  )
		{
			case 1 :
				q.do_register(connectfd, &msg, db);    
				break;
			case 2 :
				q.do_login(connectfd, &msg, db);
				break;
			case 3 :
				q.do_changepass(connectfd,&msg,db);
				break;
		}


	}
	cout<<"client quite!!!"<<endl;
	exit(0);
	return;
} 
int main(int argc, char *argv[])
{
	int listenfd, connectfd;
	pid_t pid;
	sqlite3 *db;
	dbserver d;
	d.usr();

	if (sqlite3_open(DATABASE, &db) != SQLITE_OK)
	{
		cout<<"error : "<<sqlite3_errmsg(db)<<endl;
		exit(-1);
	/*char sql1[128];
	char buf1[100]={0};
	char *errmsg1;
	system("sqlite3 my.db");
	sqlite3 *db1;
	if(sqlite3_open(DATABASE,&db1)!= SQLITE_OK)
	{
		goto next1;
	}
	sprintf(sql1, "create table usr (name char,data char);");
	if(sqlite3_exec(db, sql1, NULL, NULL, &errmsg1)!=SQLITE_OK)
	{
		cout<<"create no ok ,please delete my.db"<<endl;
		sqlite3_free(errmsg1);
	}
        else
	{
		cout<<"my.db usr create ok!!!"<<endl;
	}
	*/
	}
	if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("fail to socket");
		exit(-1);
	}
	struct sockaddr_in server_addr,clientaddr ;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	server_addr.sin_port = htons(8888);
	int len = sizeof server_addr;
	if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("fail to bind");
		exit(-1);
	}
	cout<<"bind....."<<endl;


	if (listen(listenfd, 5) < 0)
	{
		perror("fail to listen");
		exit(-1);


	}
	cout<<"listen.............."<<endl;
	signal(SIGCHLD, SIG_IGN);  // 为了不生成僵尸进程
	while (1)
	{
		if ((connectfd = accept(listenfd, NULL , NULL)) < 0)
		{
			perror("fail to accept");
			exit(-1);
		}
		printf("incoming: %s\n", inet_ntoa(clientaddr.sin_addr) );
		if ((pid = fork()) < 0)         //多进程
		{
			perror("fail to fork");
			exit(-1);


		}
		if (pid == 0)             //子进程
		{
			do_client(connectfd, db);


		}
		close(connectfd);
	}
	return 0;
}


