#ifndef MGRAPH_H
#define MGRAPH_H
#include "base.h"
typedef struct ArcCell{
    int adj;  //VRType是顶点关系类型，无权图：1 相邻,0：不相邻；有权图：权值
    InfoType *info;  //该弧相关信息的指针
}ArcCell,AdjMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];  //Adjacency Matrix 邻接矩阵
typedef struct{
    VertexType vexs[MAX_VERTEX_NUM];   //顶点向量
    AdjMatrix arcs;    //弧所构成的矩阵
    int vexnum,arcnum;   //顶点个数和弧或边的个数
    int kind;   //图的种类
}MGraph;
Status CreateMGraph(MGraph &G);
Status CreateUDN_M(MGraph &G);
Status CreateUDG_M(MGraph &G);
Status CreateDN_M(MGraph &G);
Status CreateDG_M(MGraph &G);
int LocateVex(MGraph &G,VertexType vex);
void DFSTraverse(MGraph G,Status(*Visit)(VertexType vex));  //邻接矩阵的深度优先遍历
void DFS(MGraph G,int vex,Status(*Visit)(VertexType vex));   //对每个顶点的邻接点进行遍历的递归函数
#endif // MGRAPH_H
