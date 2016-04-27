#include "declare.h"
void init_stack(Stack *stack)
{
    if (stack == NULL) return;
    stack->pnode = (Node *)malloc(sizeof(Node));
    assert (stack->pnode != NULL);

    stack->top = stack->pnode;
}
void push(Stack *stack, Web *d)
{
    if (d == NULL) return;
    Node *p = (Node *)malloc(sizeof(Node));
    assert (p  != NULL);
    p->data = d;
    p->next = NULL;
    p->prev = stack->top;
    stack->top->next = p;
    stack->top = p;
}
void pop(Stack *stack)
{
    if (stack == NULL || stack->top == stack->pnode) return;
    Node *prev = stack->top->prev;
    free(stack->top);
    prev->next = NULL;
    stack->top = prev;
}
Web *top(Stack *stack)
{
    if (stack == NULL || stack->top == stack->pnode) return NULL;
    return stack->top->data;
}
void destory_stack(Stack *stack)
{
    Node *p = stack->top;
    while (p != stack->pnode)
    {
        Node *tmp = p;
        p = p->prev;
        free(tmp);
    }
    free(stack->pnode);
}
