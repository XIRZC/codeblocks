#ifndef ALGRAPH_H
#define ALGRAPH_H
#include "base.h"
typedef struct ArcNode{
    int adjvex;   //该弧所指向的弧头的顶点位置
    struct ArcNode *nextarc;    //该弧指向的下一条弧的指针
    InfoType *info;  //该弧相关信息的指针
}ArcNode;
typedef struct VNode{  //顶点结点类型
    VertexType data;  //顶点信息
    ArcNode *firstarc;   //指向第一条依附该顶点的弧的指针
}VNode,AdjList[MAX_VERTEX_NUM];
typedef struct{
    AdjList vexs;  //邻接表的头结点数组
    int vexnum,arcnum;
    int kind;
}ALGraph;
int LocateVex_AL(ALGraph &G,VertexType vex);
Status CreateUDN_AL(ALGraph &G);
Status CreateDN_AL(ALGraph &G);
Status CreateUDG_AL(ALGraph &G);
Status CreateDG_AL(ALGraph &G);
Status CreateALGraph(ALGraph &G);
void DFSTraverse_AL(ALGraph G,Status(*Visit)(VertexType vex)); //邻接表方式的深度优先遍历
void DFS_AL(ALGraph G,int vex,Status(*Visit)(VertexType vex)); //对每个顶点的邻接点进行遍历的递归函数
#endif // ALGRAPH_H
