#include <stdio.h>
#include <string.h>
//用于处理calculate提交的get请求
int main()
{
    char *buf,*p;
    char content[8192];
    char cal;
    int n1,n2,num;
    scanf("n1=%d&cal=%c&n1=%d",&n1,&cal,&n2);
    sprintf(content,"<h2>Welcome to the calculation<h2>");
    if(cal=='+')
    {
        num=n1+n2;
        sprintf(content,"%sYou select the adder\r\n<p>",content);
        sprintf(content,"%sThe answer is :%d+%d=%d\r\n<p>",content,n1,n2,num);
    }
    else if(cal=='-')
    {
        num=n1-n2;
        sprintf(content,"%sYou select the subtract\r\n<p>",content);
        sprintf(content,"%sThe answer is :%d-%d=%d\r\n<p>",content,n1,n2,num);
    }
    else if(cal=='*')
    {
        num=n1*n2;
        sprintf(content,"%sYou select the multiply\r\n<p>",content);
        sprintf(content,"%sThe answer is :%d*%d=%d\r\n<p>",content,n1,n2,num);

    }
    else
    {
        num=n1/n2;
        sprintf(content,"%sYou select the divide\r\n<p>",content);
        sprintf(content,"%sThe answer is :%d/%d=%d\r\n<p>",content,n1,n2,num);

    }
    printf("Content-length:%d\r\n",strlen(content));
    printf("Content-type:text/html\r\n\r\n");
    printf("%s",content);
    fflush(stdout);
    return 0;
}
