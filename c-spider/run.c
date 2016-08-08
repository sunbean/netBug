#include "declare.h"
/*get one link from stack, */
int web_num = 0;
void *child_do(void *fd);
void parent_do(int *fd, char *);
Web * remove_from(int *arr, Web ** set, int i, int max)
{
    Web *res = set[i];
    for (; i<max; ++i)
    {
        arr[i] = arr[i+1];
        set[i] = set[i+1];
    }
    return res;
}
int main (int argc, char **argv)
{
    char *str = (char *)calloc(strlen(argv[1])+1, sizeof(char));
    assert (str != NULL);
    strcpy(str, argv[1]);
    g_link = (Web *)calloc(1, sizeof(Web));
    assert (g_link != NULL);
    parse_link(argv[1], g_link);
    init_stack(&stack);

    int fd[2];
    if (pipe(fd) < 0) 
    {
        perror("pipe");
        return -1;
    }
    push(&stack, g_link);
    fd_set read_fd;
    FD_ZERO(&read_fd);
    web_num = 0;
    while (web_num < 1000)
    {
        static int i = 0;
        int _read[MAX] = {0};
        Web *set[MAX];
        show(&stack);
        while (!empty(&stack) && i<MAX)
        {
            Web *p = top(&stack);
            _read[i] = communicate_web(p);
            set[i] = p;
            pop(&stack);
            FD_SET(_read[i], &read_fd);
            ++i;
        }
        if (i == 0) continue;
        select(_read[i-1]+1, &read_fd, NULL, NULL, NULL);
        int j = 0;
        for (j=0; j<i+1; ++j)
        {
            if(FD_ISSET(_read[j], &read_fd))
            {
                char buff[64] = END_MSG;
                get_msg_from_fd(fd[1], _read[j]);
                write(fd[1], buff, strlen(buff));
                write(fd[1], buff, strlen(buff));
                FD_CLR(_read[j], &read_fd);
                --i;
                Web *p = remove_from(_read, set,  j, i);
                p->child = (Web *)calloc(1, sizeof(Web));
                assert (p->child != NULL);
                parse_html(fd, p->child);
                ++web_num;
            }
        }
    }
    return 0;
}
