#include "declare.h"

int main()
{
    struct hostent * p = gethostbyname("www.cnblogs.com");
    if (p == NULL) 
    {
        perror("gethostbyname");
        exit(-1);
    }
    char str[32] = "";
    inet_ntop(p->h_addrtype, *(p->h_addr_list), str, sizeof(str));

    struct sockaddr_in sockname;
    memset(&sockname, 0, sizeof(sockname));
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) perror("socket");
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(80);
    sockname.sin_addr.s_addr = inet_addr(str);
    if (connect(sockfd, 
                (struct sockaddr *)&sockname, 
                sizeof(sockname))<0)
        perror("connect");
    printf("connect with one web\n");
    char *msg = (char *)calloc(1024, sizeof(char));
    assert(msg != NULL);
    strcpy(msg, "HEAD ");
    strcat(msg, "http://www.cnblogs.com/wawlian/archive/2012/06/18/2553061.html");
    strcat(msg, " HTTP/1.1\r\n");
    strcat(msg, "HOST: ");
    strcat(msg, "www.cnblogs.com");
    strcat(msg, "\r\nConnection: closed");
    strcat(msg, "\r\n\r\n");

    send(sockfd, msg, strlen(msg), 0);
    char getBuff[256] = "";
    int num  = 0;
    while ((num=read(sockfd, getBuff, 255)) > 0)
    {
        printf("%s\n", getBuff);
        memset(getBuff, 0, sizeof(getBuff)*sizeof(char));
    }

    return 0;
}
