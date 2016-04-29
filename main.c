#include "declare.h"


int main()
{
    int fd = open("doc/file", O_RDONLY);
    if (fd == -1) perror("open");
    g_link = (Web *)malloc(sizeof(Web)*1);
    g_link->next = NULL;
    assert (g_link != NULL);
    parse_html(&fd, g_link);
    close(fd);
    show_web();
    return 0;
}
