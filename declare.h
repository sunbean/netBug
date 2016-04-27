#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>

typedef struct Web
{
    char *dest;
    char *host;
    char *ip;
    struct Web *next;
    struct Web *child;
}Web;


typedef struct Node 
{
    Web *data;
    struct Node *next;
    struct Node *prev;
}Node;
typedef struct Stack
{
    Node *top;
    Node *pnode;
}Stack;
void init_stack(Stack *);
void push(Stack *, Web *);
void pop(Stack *);
Web *top(Stack *);
/*parse a web link to get dest_url, host, ip*/
void parse_link(char *weblink, Web *link);
void get_host(Web *link);
void get_ip(Web *link);
char *pack_msg(Web *link);
void communicate_web(int *fd, Web *link);

