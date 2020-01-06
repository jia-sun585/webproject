#include <stdio.h>
#include <string.h>
//用于处理introduce_get提交的get请求
int main()
{
    char *buf,*p;
    char content[8192];
    String name,sex,age,native;
    scanf("name=%s&sex=%s&age=%s&native=%s",&name,&sex,&age,&native);
    sprintf(content,"<h2>欢迎您访问本网站<h2>");
    sprintf(content,"%s您的名字叫%s,性别%s,来自%s,今年%s岁",content,name,sex,native,age);

    return 0;
}
