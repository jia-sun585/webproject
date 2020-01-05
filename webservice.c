#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include "rio.h"
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
//服务客户端
void *serve_cilent(void *vargp);
int main(int argc,char *argv[])
{
    int listen_sock,conn_sock,port,clientlen;
    struct sockaddr_in clientaddr;
    if(argc!=2)
    {
        fprintf(stderr,"usage:%s<port>\n",argv[0]);
        return 0;
    }
    port=atoi(argv[1]);
    listen_sock=open_listen_sock(port);
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
//初始化服务器套接字的地址结构
bzero((char*)&serveraddr,sizeof(serveraddr));
serveraddr.sin_family=AF_INET;
serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
serveraddr.sin_port=htoms((unsigned short)port);
//绑定套接字
if(bind(listen_sock,(SA *)&serveraddr,sizeof(serveraddr))<0)
    return -1;
//监听套接字
if(listen(listen_sock,LISTENQ)<0)
    return -1;
return listen_sock;
}
int is_static(char *uri)
{
if(!strstr(uri,"cgi-bin"))
    return 1;
}
void error_request(int fd,char *cause,char *errnum,char *cue,char *description)
{
 char buf[1024],body[1024];
 //构建http响应
sprintf(body,"<html><title>error request</title>");
sprintf(body,"%s<body>\r\n",body);
sprintf(body,"%s%s:%s\r\n",body,errnum,cue);
sprintf(body,"%s<p>%s: %s\r\n",body,description,cause);
sprintf(body,"%s<hr><em>Web server</em>\r\n",body);
//发送响应
sprintf(buf,"HTTP/1.0%s%s\r\n",errnum,cue);
rio_writen(fd,buf,strlen(buf));
sprintf(buf,"Content-type:text/html\r\n");
rio_writen(fd,buf,strlen(buf));
sprintf(buf,"Content-length:%d\r\n\r\n",(int)strlen(body));
rio_writen(fd,buf,strlen(buf));
rio_writen(fa,body,strlen(body));
}
void getfiletype(char *filename,char *filetype)
{
if(strstr(filename,".html"))
    strcpy(filetype,"text/html");
else if(strstr(filename,".jpg"))
    strcpy(filetype,"image/jpeg");
else if(strstr(filename,".mpeg"))
    strcpy(filetype,"video/mpeg");
else strcpy(filetype,"text/html");
}
void analyze_static_uri(char *uri,char *filename)
{
    char *ptr;
    strcpy(filename,".");
    strcat(filename,uri);
    //默认访问index.html
    if(uri[strlen(uri)-1]=='/')
        strcat(filename,"index.html");
}
void service_static(int fd,char *filename,int filesize)
{
    int srcfd;
    char *srcp,filetype[8192],buf[8192];
    getfiletype(filename,filetype);
    //发送响应头
    sprintf(buf,"HTTP/1.0 200 OK\r\n");
    sprintf(buf,"%sServer:Web server\r\n",buf);
    sprintf(buf,"%sContent-length:%d\r\n",buf,filesize);
    sprintf(buf,"%sContent-type:%s\r\n\r\n",buf,filetype);
    rio_writen(fd,buf,strlen(buf));
    //发送响应体
    srcfd=open(filename,O_RDONLY,0);
    srcp=mmap(0,filesize,PROT_READ,MAP_PRIVATE,srcfd,0);
    close(srcfd);
    rio_writen(fd,srcp,filesize);
    munmap(srcp,filesize);



}
void *serve_cilent(void *vargp)
{
    int conn_sock=*((int *)vargp);

}
