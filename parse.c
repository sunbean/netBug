#include "declare.h"
void parse_html(int *fd, Web *pnode)
{
    int num = 0;
    char buff[256] = "";
    char mem[64] = "";
    bool flag = true;
    while ((num=read(fd[0], buff, sizeof(char)*255)) > 0)
    {
        char *p = buff;
        if (!flag) 
        {
            char *q = buff;
            int i = strlen(mem);
            for (; *q != '\"'; ++q, ++i)
            {
                mem[i] = *q;
            }
            //add_in_web(pnode, mem);
            memset(mem, 0, sizeof(char)*64);
            flag = true;
        }
        while ((p=strstr(p, "href=\"")) != NULL)
        {
            p = p+6;
            char *q = mem;
            while (*p != '\"' && *p != 0) *q++ = *p++;
            if (*p == 0) flag = false;    //cur time unfinish
            else
            {
               // add_in_web(pnode, mem);  //todo
                memset(mem, 0, sizeof(char)*64);
            }
        }
        memset(buff, 0, sizeof(char)*256);
    }
}
char *pack_msg(Web *link)
{
    char *msg = (char *)calloc(1024, sizeof(char));
    assert(msg != NULL);
    strcpy(msg, "GET ");
    strcat(msg, link->dest);
    strcat(msg, " HTTP/1.1\r\n");
    strcat(msg, "HOST: ");
    strcat(msg, link->host);
    strcat(msg, "\r\nConnection: closed");
    strcat(msg, "\r\n\r\n");
    return msg;
}
void communicate_web(int *fd, Web *link)
{
    struct sockaddr_in sockname;
    memset(&sockname, 0, sizeof(sockname));
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) perror("socket");
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(80);
    sockname.sin_addr.s_addr = inet_addr(link->ip);
    if (connect(sockfd, 
                (struct sockaddr *)&sockname, 
                sizeof(sockname))<0)
        perror("connect");
    printf("connect with one web\n");
    char *msg = pack_msg(link);
    send(sockfd, msg, strlen(msg), 0);
    char getBuff[256] = "";
    int num = 0;
    while ((num=read(sockfd, getBuff, 255)) > 0)
    {
        write(fd[1], getBuff, sizeof(char)*num);
        memset(getBuff, 0, sizeof(getBuff)*sizeof(char));
    }
}
void get_ip(Web *link)
{
    struct hostent * p = gethostbyname(link->host);
    if (p == NULL) 
    {
        perror("gethostbyname");
        exit(-1);
    }
    char str[32] = "";
    inet_ntop(p->h_addrtype, *(p->h_addr_list), str, sizeof(str));
    link->ip = (char *)calloc(strlen(str), sizeof(char));
    assert(link->ip != NULL);
    strcpy(link->ip, str);
}
void get_host(Web *link)
{
    char *buff = (char *)calloc(strlen(link->dest)+1, sizeof(char));
    assert(buff != NULL);
    strcpy(buff, link->dest);

    char *p = buff + strlen("http://");
    p = strtok(p, "/");
    link->host = (char *)calloc(strlen(p), sizeof(char));
    assert (link->host != NULL);
    strcpy(link->host, p);
    free(buff);
}
void parse_link(char *weblink, Web *link)
{
    char *dest = NULL;
    if (strstr(weblink, "http://") == NULL)
    {
        dest = (char *)calloc(strlen(weblink)+strlen("http://")+1, sizeof(char));
        assert(dest != NULL);
        strcpy(dest, "http://");
        strcat(dest, weblink);
    }
    else
    {
        dest = (char *)calloc(strlen(weblink)+1, sizeof(char));
        assert(dest != NULL);
        strcpy(dest, weblink);
    }
    link->dest = dest;
    get_host(link);
    get_ip(link);
}
