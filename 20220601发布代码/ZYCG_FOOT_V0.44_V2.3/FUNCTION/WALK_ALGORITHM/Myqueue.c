#include "Myqueue.h"
#include "math.h"
#include<stdbool.h>


void init_que(Que_typedef *q) //初始化队列,填满队列，将所有元素值设为0
{
    q->front=q->rear=0;
    while(!(q->front==(q->rear+1)%MAXSIZE))
    {
        q->data[q->rear]= 0.0f;
        q->rear=(q->rear+1)%MAXSIZE;
    }
}

void init_que_small(Que_typedef_small *q) //初始化队列,填满队列，将所有元素值设为0
{
    q->front=q->rear=0;
    while(!(q->front==(q->rear+1)%MAXSIZES))
    {
        q->data[q->rear]= 0.0f;
        q->rear=(q->rear+1)%MAXSIZES;
    }
}



void init_que_accel(Que_typedef_accel *q) //初始化队列,填满队列，将所有元素值设为0
{
    q->front=q->rear=0;
    while(!(q->front==(q->rear+1)%MAXSIZE_A))
    {
        q->data0[q->rear]= 0.0f;
        q->data1[q->rear]= 0.0f;
        q->data2[q->rear]= 0.0f;
        q->data[q->rear]= 0.0f;
        q->rear=(q->rear+1)%MAXSIZE_A;
    }
}




bool Empty(Que_typedef *q) //判断队列是否为空
{
    if(q->front==q->rear)
    {
        return true;
    }
    else
    {
        return false;
    }
}




bool full(Que_typedef *q)  //判断是否已满
{
    if((q->rear+1)%MAXSIZE==q->front)
    {
        return true;
    }
    else
    {
        return false;
    }
}



