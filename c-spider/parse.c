#include "declare.h"
void judge_web_link(char **str, int size)
{
    int i = 0;
    for (; i<size; ++i)
    {
        if (str[i] != NULL)
        {
            char *sstr = (char *)malloc(strlen(str[i])+1);
            assert (sstr != NULL);
            strcpy(sstr, str[i]);
            char *host = strstr(sstr, "http://")+7;
            host = strtok(host, "/");
            //printf("%s\n", host);

            struct hostent * p = gethostbyname(host);
            if (p == NULL) 
            {
                perror("gethostbyname");
                exit(-1);
            }
            char ipname[32] = "";
            inet_ntop(p->h_addrtype, *(p->h_addr_list), ipname, sizeof(ipname));
            struct sockaddr_in sockname;
            memset(&sockname, 0, sizeof(sockname));
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd == -1) perror("socket");
            sockname.sin_family = AF_INET;
            sockname.sin_port = htons(80);
            //printf("%s\n", ipname);
            sockname.sin_addr.s_addr = inet_addr(ipname);
            if (connect(sockfd, 
                        (struct sockaddr *)&sockname, 
                        sizeof(sockname))<0)
                perror("connect");
            char *msg = (char *)calloc(1024, sizeof(char));
            assert(msg != NULL);
            strcpy(msg, "HEAD ");
            strcat(msg, str[i]);
            strcat(msg, " HTTP/1.1\r\n");
            strcat(msg, "HOST: ");
            strcat(msg, host);
            free(sstr);
            strcat(msg, "\r\nConnection: closed");
            strcat(msg, "\r\n\r\n");
            send(sockfd, msg, strlen(msg), 0);
            free(msg);
            char getBuff[256] = "";
            read(sockfd, getBuff, 255);
            char *tmp = strstr(getBuff, " ");
            ++tmp;
            if (strncmp(tmp, SUC, 3) != 0)
            {
                free(str[i]);
                str[i] = NULL;
            }
        }
    }
}
void reduce_same_web(char **str, int size)
{
    int i = 0;
    for (; i<size; ++i)
    {
        int j = i+1;
        for (; j<size && str[i] != NULL; ++j)
        {
            if (str[j] != NULL && strcmp(str[j], str[i]) == 0)
            {
                free(str[j]);
                str[j] = NULL;
            }
        }
    }
}
bool is_exist(Web *link, char *str)
{
    while (link != NULL && link->dest != NULL)
    {
        if (strcmp(str, link->dest) == 0 ||
            (link->child != NULL && is_exist(link->child, str)))
        {
            //free(str);
            return true;
        }
        link = link->next;
    }
    return false;
}
void insert_link(Web *link, char *str)
{
    /*deal with first node*/
    if (link->dest == NULL)
    {
        parse_link(str, link);
        push(&stack, link);
    }
    /*deal with other node*/
    else 
    {
        Web *next = link->next;
        link->next = (Web *)calloc(1, sizeof(Web));
        assert(link->next != NULL);
        /*through str get link msg and fill in link->next*/
        parse_link(str, link->next);
        link->next->next = next;
        push(&stack, link->next);
    }
}
void add_into_web(Web *g_link, Web *l_link, char **str, int size)
{
    int i = 0;
    for (; i<size; ++i)
    {
        if (str[i] != NULL && !is_exist(g_link, str[i]))
        {
            insert_link(l_link, str[i]);
        }
    }
}

void parse_html(int *fd, Web *pnode)
{
    int num = 0;
    char buff[256] = "";
    char mem[128] = "";
    char *str[256];      // has limit
    int k = 0;
    bool flag = true;
    bool tag = true;
    char *end = NULL;
    while (tag && (num=read(fd[0], buff, sizeof(char)*255)) > 0)
    {
        end = strstr(buff, END_MSG);
        char *p = buff;
        if (end != NULL)
        {
            p = strtok(buff, END_MSG);
            tag = false;
        }
        if (!flag) 
        {
            char *q = buff;
            int i = strlen(mem);
            for (; *q != '\"'; ++q, ++i)
            {
                mem[i] = *q;
            }

            str[k] = (char *)malloc(strlen(mem)+1);
            assert (str[k] != NULL);
            strcpy(str[k], mem);
            ++k;

            p = q + 1;
            memset(mem, 0, sizeof(char)*64);
           flag = true;
        }
        while ((p=strstr(p, "href=\"http:")) != NULL)
        {
            p = p+6;
            char *q = mem;
            while (*p != '\"' && *p != 0) *q++ = *p++;
            if (*p == 0) flag = false;    //cur time unfinish
            else
            {
                str[k] = (char *)malloc(strlen(mem)+1);
                assert (str[k] != NULL);
                strcpy(str[k], mem);
                ++k;
                memset(mem, 0, sizeof(char)*64);
            }
        }
        memset(buff, 0, sizeof(char)*256);
    }
    if (end != NULL)
    {
        while ((end=strstr(end, "href=\"http:")) != NULL)
        {
            end = end+6;
            char *q = mem;
            while (*end != '\"' && *end != 0) *q++ = *end++;
            str[k] = (char *)malloc(strlen(mem)+1);
            assert (str[k] != NULL);
            strcpy(str[k], mem);
            ++k;
        }
    }
    if (str[0] != NULL)
    {

        reduce_same_web(str, k);
        judge_web_link(str, k);
        add_into_web(g_link, pnode, str, k);
    }
    //free_str(str, k); todo
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
int communicate_web(Web *link)
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
    //printf("connect with one web\n");
    char *msg = pack_msg(link);
    send(sockfd, msg, strlen(msg), 0);
    return sockfd;
}
void get_msg_from_fd(int w_fd, int r_fd)
{
    char getBuff[256] = "";
    int num = 0;
    read(r_fd, getBuff, 20);
    char *p = getBuff;
    p = strstr(p, " ");
    ++p;
    if (strncmp(p, SUC, 3) == 0)
    {
        while ((num=read(r_fd, getBuff, 255)) > 0)
        {
            write(w_fd, getBuff, sizeof(char)*num);
            memset(getBuff, 0, sizeof(getBuff)*sizeof(char));
        }
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
void show_web(Web *p)
{
    while (p != NULL)
    {
        printf("%s\n", p->dest);
        if (p->child != NULL) show_web(p->child);
        p = p->next;
    }
}
