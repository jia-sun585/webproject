#include <stdio.h>
//open_listen_sock辅助函数，用于监听等待服务端的请求，返回一个监听描述符
int open_listen__sock(int port);
//用于http事务处理
void http_trans(int fd);
int is_static(char *uri);
int main()
{
	return 0;
}
