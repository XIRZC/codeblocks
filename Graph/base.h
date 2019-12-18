#ifndef BASE_H
#define BASE_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>  //INT_MAX的所在库
#include<string.h>
#define OVERFLOW -2
#define OK 1
#define ERROR 0
#define INFINITY 1000000
#define MAX_VERTEX_NUM 20
#define STACK_INIT_SIZE 50
#define STACKINCREMENT 10
typedef int Status;
typedef int InfoType;
typedef char VertexType;
typedef int SElemType;
typedef struct SqStack{
	SElemType *base;
	SElemType *top;
	int stackSize;
}SqStack;
typedef struct Edge{
    int lowcost;
    VertexType adjvex;
}Edge;  //代表一整条边，下标为一个结点，adj域为与该点邻接的权值最小的邻接点，lowcost对应权值
Status VexOutput(VertexType vex);
Status InitStack(SqStack &S);  //栈相关函数
Status Pop(SqStack &S,SElemType &del);
Status Push(SqStack &S,SElemType add);
Status StackEmpty(SqStack &S);
#endif
