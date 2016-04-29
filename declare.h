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
#include <stdbool.h>
#define MAX 3
typedef struct Web
{
    char *dest;
    char *host;
    char *ip;
    struct Web *next;
    struct Web *child;
}Web;
/*a global web type var for the all link*/
Web *g_link;
/*a simple and useful stack*/
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
/*a stack*/
Stack stack;
void init_stack(Stack *);
void parse_html(int *fd, Web *pnode);
void push(Stack *, Web *);
void pop(Stack *);
Web *top(Stack *);
int count(Stack*);
bool empty(Stack *stack);


/*parse a web link to get dest_url, host, ip*/
void parse_link(char *weblink, Web *link);
void get_host(Web *link);
void get_ip(Web *link);
char *pack_msg(Web *link);
int communicate_web(int *fd, Web *link);
/*over parse add the msg into Web*/
void add_into_web(Web *g_link, Web *l_link, char **str, int size);
void insert_link(Web *link, char *str);
bool is_exist(Web *link, char *str);
void reduce_same_web(char **str, int size);
void show_web();
