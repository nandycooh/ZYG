#ifndef __MYQUEUE_H
#define __MYQUEUE_H

#include<stdbool.h>

#define MAXSIZE 4
#define MAXSIZES 501
#define MAXSIZE_A 301
//#define MAXSIZES 201
//#define MAXSIZE_A 101

//定义队列结构体
typedef struct 
{
    float data[MAXSIZE];
    int front;
    int rear;
}Que_typedef;

typedef struct 
{
    float data[MAXSIZES];
    int front;
    int rear;
}Que_typedef_small;

typedef struct 
{
    float data0[MAXSIZE_A];
    float data1[MAXSIZE_A];
    float data2[MAXSIZE_A];
    float data[MAXSIZE_A];
    int front;
    int rear;
}Que_typedef_accel;




//判断队列是否为空
bool Empty(Que_typedef *q);

//判断队列是否已满
bool full(Que_typedef *q);

//初始化队列
void init_que(Que_typedef *q);
void init_que_small(Que_typedef_small *q);
void init_que_accel(Que_typedef_accel *q);




#endif
