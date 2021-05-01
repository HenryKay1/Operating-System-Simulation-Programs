#include <stdlib.h>
#define TRUE 1
#define FALSE 0
#define FULL 2

struct fifoNode
{
    int job;
    struct node *next;
};
typedef struct fifoNode node;

struct queue
{
    int count;
    node *front;
    node *rear;
};
typedef struct queue queue;

void initialize(queue *Q)
{
    Q->rear = NULL;
 
    Q->front = NULL;
       Q->count = 0;
    
}

int isempty(queue *Q)
{
    return (Q->rear == NULL);
}

void enqueue(queue *Q, int result)
{
    
        node *tmp;
        tmp = malloc(sizeof(node));
        tmp->job = result;
        tmp->next = NULL;
        if(!isempty(Q))
        {
            Q->rear->next = tmp;
            Q->rear = tmp;
        }
        else
        {
            Q->front = Q->rear = tmp;
        }
        Q->count++;
    
}

int dequeue(queue *Q)
{
    node *tmp;
    int n = Q->front->job;
    tmp = Q->front;
    Q->front = Q->front->next;
    Q->count--;
    free(tmp);
    return(n);
}

void display(node *head)
{
    if(head == NULL)
    {
        printf("NULL\n");
    }
    else
    {
        printf("%d\n", head -> job);
        display(head->next);
    }
}


