#include <stdio.h>
//open_listen_sock辅助函数，用于监听等待服务端的请求，返回一个监听描述符
int open_listen__sock(int port);
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
