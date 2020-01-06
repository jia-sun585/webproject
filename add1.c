#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *buf, *p;
    char content[8192];
    int n1=0, n2=0;

    /* Extract the two arguments from standard input */
    scanf("num1=%d&num2=%d", &n1, &n2);

    /* Make the response body */
    sprintf(content, "Welcome to add.com: ");
    sprintf(content, "%sTHE Internet addition portal.\r\n<p>", content);
    sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>",
            content, n1, n2, n1 + n2);
    sprintf(content, "%sThanks for visiting!\r\n", content);

    /* Generate the HTTP response */
    printf("Content-length: %d\r\n", strlen(content));
    printf("Content-type: text/html\r\n\r\n");
    printf("%s", content);
    fflush(stdout);
    exit(0);
}
