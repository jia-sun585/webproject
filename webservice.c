#include <stdio.h>
//open_listen_sock辅助函数，用于监听等待服务端的请求，返回一个监听描述符
int open_listen__sock(int port);
//用于http事务处理
void http_trans(int fd);
//判断该请求是否为静态页面请求
int is_static(char *uri);
//解析静态内容请求uri
void analyze_static_uri(char *uri,char *filename);
void analyze_dynamic_uri(char *uri,char *filename,char *arg);
int main()
{
	return 0;
}
