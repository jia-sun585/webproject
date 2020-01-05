#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <malloc.h>
#include "rio.h"
#define LISTENQ  1024
typedef struct sockaddr SA;
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
    int listen_sock,*conn_sock,port,clientlen;
    pthread_t tid;
    struct sockaddr_in clientaddr;
    if(argc!=2)
    {
        fprintf(stderr,"usage:%s<port>\n",argv[0]);
        return 0;
    }
    port=atoi(argv[1]);
    listen_sock=open_listen_sock(port);
    while(1)
    {
        clientlen=sizeof(clientaddr);
        conn_sock=malloc(sizeof(int));
        *conn_sock=accept(listen_sock,(SA *)&clientaddr,&clientlen);
        pthread_create(&tid,NULL,serve_cilent,conn_sock);
    }
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
serveraddr.sin_port=htons((unsigned short)port);
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

void analyze_dynamic_uri(char *uri,char *filename,char *args)
{
    char *ptr;
    ptr = index(uri, '?');
    if(ptr) {
        strcpy(args, ptr+1);
        *ptr = '\0';
    }
    else
        strcpy(args,"");
    strcpy(filename, ".");
    strcat(filename, uri);
}

void service_dynamic(int fd,char *filename,char *args)
{
    char buf[8192], *emptylist[] = {NULL};
    int pfd[2];

    /*返回http响应头部*/
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server:Web Server\r\n");
    rio_writen(fd, buf, strlen(buf));

    pipe(pfd);
    /*子进程处理*/
    if(fork() == 0) {
        close(pfd[1]);
        dup2(pfd[0], STDIN_FILENO);
        /*重定向标准输出到客户端*/
        dup2(fd, STDOUT_FILENO);
        /*运行CGI项目*/
        execve(filename, emptylist, environ);
    }

    close(pfd[0]);
    write(pfd[1], args, strlen(args)+1);
    /*父进程等待cgi子进程结束并回收*/
    wait(NULL);
    close(pfd[1]);

}


void http_trans(int fd)
{
    int static_flag;
    struct stat sbuf;
    char buf[8192], method[8192], uri[8192], version[8192];
    char filename[8192], cgiargs[8192];
    rio_t rio;

    /*读取请求头*/
    rio_readinitb(&rio, fd);
    rio_readlineb(&rio, buf, 8192);
    sscanf(buf, "%s %s %s", method, uri, version);

    /*判断提交方法*/
    if(strcasecmp(method, "GET") && strcasecmp(method, "POST")) {
        error_request(fd, method, "501", "Not Implemented", "webserver does not implement this method");
        return ;
    }

    /*判断请求的是静态页面还是动态页面*/
    static_flag = is_static(uri);

    /*分析是否有所请求的页面*/
    if(static_flag)
        analyze_static_uri(uri, filename);
    else
        analyze_dynamic_uri(uri, filename, cgiargs);


    if(stat(filename, &sbuf) < 0) {
        error_request(fd, filename, "404", "Not found",
                      "webserver could not find this file");
        return ;
    }

    /*返回静态页面*/
    if(static_flag) {
        if(!(S_ISREG(sbuf.st_mode))||!(S_IRUSR & sbuf.st_mode)) {
            error_request(fd, filename, "403", "Forbidden",
                          "webserver is not permtted to read the file");
            return ;
        }
        service_static(fd, filename, sbuf.st_size);
    }
    /*返回动态页面*/
    else {
        if(!(S_ISREG(sbuf.st_mode))||!(S_IRUSR & sbuf.st_mode)) {
            error_request(fd, filename, "403", "Forbidden",
                          "webserver could not run the CGI program");
            return ;
        }
        service_dynamic(fd, filename, cgiargs);
    }
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
    pthread_detach(pthread_self());
    free(vargp);
    http_trans(conn_sock);
    close(conn_sock);
}
