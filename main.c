#include "declare.h"



/*stack and g_link is global var*/
int main()
{
    init_stack(&stack);
    int fd = open("doc/file", O_RDONLY);
    if (fd == -1) perror("open");
    char *arg = "http://www.cnblogs.com/wawlian/archive/2012/06/18/2553061.html";

    char *str = (char *)calloc(strlen(arg)+1, sizeof(1));
    assert (str != NULL);
    strcpy(str, arg);
    g_link = (Web *)calloc(1, sizeof(Web));
    assert (g_link != NULL);

    insert_link(g_link, str);
    g_link->child = (Web *)calloc(1, sizeof(Web));
    assert(g_link != NULL);
    parse_html(&fd, g_link->child);

    close(fd);
    show_web(g_link);
    printf("the stack size is %d\n", count(&stack));
    return 0;
}
