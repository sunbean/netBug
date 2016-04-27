#include "declare.h"
/*get one link from stack, */
void child_do(Web *link, int *fd, Stack *stack);
void parent_do(Web *link, int *fd, Stack *stack);
void child_do(Web *link, int *fd, Stack *stack)
{
//todo
}
void parent_do(Web *link, int *fd, Stack *stack)
{
    //todo
}
int main ()
{
    char weblink[64] = "http://www.cnblogs.com/wawlian/archive/2012/06/18/2553061.html" ;
    Web *pnode = (Web *)malloc(sizeof(Web));
    assert(pnode != NULL);
    parse_link(weblink, pnode);
    Stack stack;
    init_stack(&stack);
    int fd[2];
    if (pipe(fd) < 0)
    {
        perror("pipe");
    }
    /*pipe may be not useful*/
    communicate_web(fd, pnode);
    parse_html(fd, pnode);
    int pid = fork();
    /*child*/
    if (pid == 0)
    {
        child_do(pnode, fd, &stack);
    }
    /*parent*/
    else
    {
        parent_do(pnode, fd, &stack);
    }
    return 0;
}
