#include "declare.h"
/*get one link from stack, */
void child_do(int *fd);
void parent_do(int *fd, char *);
void remove_from(int *arr, int i, int max)
{
    for (; i<max; ++i)
    {
        arr[i] = arr[i+1];
    }
}

void *child_do(void *p)
{
    /*through pipe to write data*/
    int *fd = (int *)p;

    fd_set read_fd;
    FD_ZERO(&read_fd);
    while (1)
    {
        static int i = 0;
        int _read[MAX-1] = {0};
        if (!empty(&stack) && i<MAX)
        {
            Web *p = top(&stack);
            pop(&stack);
            _read[i] = communicate_web(p); 
            FD_SET(_read, &read_fd);
            ++i;
        }
        if (i == 0) continue;
        select(_read+1, &read_fd, NULL, NULL, NULL);
        for (int j=0; j<i+1; ++j)
        {
            if(FD_ISSET(_read[j], &read_fd))
            {
                get_msg_from_fd(fd[1], _read[j]);
                FD_CLR(_read[j], &read_fd);
                remove_from(_read, j, i);
                --i;
            }
        }
    }
    return NULL;
}
void parent_do(int *fd, char *str)
{
    insert_link(g_link, str);
    push(&stack, g_link);
    /*construct child local*/
    g_link->child = (Web *)calloc(1, sizeof(Web));
    assert(g_link != NULL);
    parse_html(fd, g_link->child);
}
int main ()
{
    char weblink[64] = "http://www.cnblogs.com/wawlian/archive/2012/06/18/2553061.html" ;
    char *str = (char *)calloc(weblink+1, sizeof(char));
    assert (str != NULL);
    srrcpy(str, weblink);
    g_link = (Web *)calloc(1, sizeof(Web));
    assert (g_link != NULL);
    parse_link(weblink, g_link);
    init_stack(&stack);
    int fd[2];
    if (pipe(fd) < 0) 
    {
        perror("pipe");
        return -1;
    }
    pthread_t child_id;
    pthread_create(&child_id, NULL, child_do, fd);
    parent_do(fd, str);
    pthread_join(child_id, NULL);
    return 0;
}
