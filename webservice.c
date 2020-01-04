#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
//open_listen_sock辅助函数，用于监听等待服务端的请求，返回一个监听描述符
int open_listen_sock(int port);
//用于http事务处理
void http_trans(int fd);
//判断该请求是否为静态页面请求
int is_static(char *uri);
//解析静态内容请求uri
void analyze_static_uri(char *uri,char *filename);
//解析动态请求uri，第三个参数是请求的参数
void analyze_dynamic_uri(char *uri,char *filename,char *args);
//服务静态内容
void service_static(int fd,char *filename,int filesize);
//服务动态内容
void service_dynamic(int fd,char *filename,char *args);
//处理错误请求，返回错误提示页面
void error_request(int fd,char *cause,char *errnum,char *cue,char *description);
//判断静态请求文件的类型
void getfiletype(char *filename,char *filetype);
int main()
{
	return 0;
}
int open_listen__sock(int port)
{
int listen_sock,optval=1;
struct sockaddr_in serveraddr;
//创建套接字
if((listen_sock=socket(AF_INET,SOCK_STREAM,0))<0)
    return -1;
if(setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,(const void*)&optval,sizeof(int))<0)
    return -1;
bzero((char*)&serveraddr,sizeof(serveraddr));
serveraddr.sin_family=AF_INET;
serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
serveraddr.sin_port=htoms((unsigned short)port);

}
