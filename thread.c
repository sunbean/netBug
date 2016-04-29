#include "declare.h"
/*get one link from stack, */
void child_do(int *fd);
void parent_do(int *fd, char *);
/*I still need to notice the msg common*/
void child_do(int *fd)
{
    /*through pipe to write data*/
    while (!empty(&stack))
    {
        int n = count(&stack);
        int num = n > MAX ? MAX : n;
        /*get some link, through IO api to deal   need sockfd*/
        //todo
    }
}
void parent_do(int *fd, char *str)
{
    insert_link(g_link, str);
    push(&stack, g_link);
    /*construct chid*/
    g_link->child = (Web *)calloc(1, sizeof(Web));
    assert(g_link != NULL);
    parse_html(fd, g_link->child);
}
int main ()
{
    char weblink[64] = "http://www.cnblogs.com/wawlian/archive/2012/06/18/2553061.html" ;
    g_link = (Web *)calloc(1, sizeof(Web));
    assert (g_link != NULL);
    parse_link(weblink, g_link);
    init_stack(&stack);
    int fd[2];
    if (pipe(fd) < 0)
    {
        perror("pipe");
    }
    /*pipe may be not useful*/
    int pid = fork();
    /*child*/
    /*use pthread, pro can't do*/
    if (pid == 0)
    {
        child_do(fd);
    }
    /*parent*/
    else
    {
        parent_do(fd, weblink);
    }
    return 0;
}
