#include "ALGraph.h"
int visited_AL[MAX_VERTEX_NUM]={0};  //顶点访问标志数组
void DFSTraverse_AL(ALGraph G,Status(*Visit)(VertexType vex))
{
    int i;
    for(i=0;i<G.vexnum;i++)
    {
        if(!visited_AL[i])
            DFS_AL(G,i,Visit);
    }
}
void DFS_AL(ALGraph G,int vex,Status(*Visit)(VertexType vex))
{
    int i=vex;
    ArcNode *temp=G.vexs[i].firstarc->nextarc;
    visited_AL[i]=1;
    Visit(G.vexs[i].data);
    while(temp!=NULL)
    {
        if(!visited_AL[temp->adjvex])
        {
            DFS_AL(G,temp->adjvex,Visit);
        }
        temp=temp->nextarc;
    }
}
int LocateVex_AL(ALGraph &G,VertexType vex)
{
    int i;
    for(i=0;i<G.vexnum;i++)
    {
        if(G.vexs[i].data==vex)
            return i;
    }
    return 0;
}
Status CreateUDN_AL(ALGraph &G)  //创建无向网
{
    int i,loc1,loc2,*weight;
    char vex1,vex2;
    ArcNode *ins=NULL;
    printf("请输入该图的顶点个数和弧的个数：");
    scanf("%d %d",&G.vexnum,&G.arcnum);
    getchar();
    printf("请依次输入各顶点：");
    for(i=0;i<G.vexnum;i++)  //顶点向量初始化
    {
        scanf("%c",&G.vexs[i].data);
        G.vexs[i].firstarc=(ArcNode*)malloc(sizeof(ArcNode));
        G.vexs[i].firstarc->nextarc=NULL;
    }
    getchar();
    printf("请输入各弧所对应的两个顶点值和该弧对应的权值：");
    for(i=0;i<G.arcnum;i++)
    {
        weight=(int *)malloc(sizeof(int));
        scanf("%c %c %d",&vex1,&vex2,weight);
        getchar();
        loc1=LocateVex_AL(G,vex1);  //只计算弧尾所对应的顶点序号
        loc2=LocateVex_AL(G,vex2);
        ins=(ArcNode*)malloc(sizeof(ArcNode));
        ins->adjvex=loc2;  //存弧头的顶点的序号
        ins->info=weight;  //存该弧所对应的权值
        ins->nextarc=G.vexs[loc1].firstarc->nextarc;  //用头插法将新结点插入
        G.vexs[loc1].firstarc->nextarc=ins;
    }
    return OK;
}
Status CreateDN_AL(ALGraph &G)   //创建有向网
{
    int i,*weight,loc1,loc2;
    char vex1,vex2;
    ArcNode *ins=NULL;
    printf("请输入该图的顶点个数和弧的个数：");
    scanf("%d %d",&G.vexnum,&G.arcnum);
    getchar();
    printf("请依次输入各顶点：");
    for(i=0;i<G.vexnum;i++)  //顶点向量初始化
    {
        scanf("%c",&G.vexs[i].data);
        G.vexs[i].firstarc=(ArcNode*)malloc(sizeof(ArcNode));
        G.vexs[i].firstarc->nextarc=NULL;
    }
    getchar();
    printf("请输入各弧所对应的两个顶点值和该弧所对应的权值：");
    for(i=0;i<G.arcnum;i++)
    {
        weight=(int *)malloc(sizeof(int));
        scanf("%c %c %d",&vex1,&vex2,weight);
        getchar();
        loc1=LocateVex_AL(G,vex1);  //只计算弧尾所对应的顶点序号
        loc2=LocateVex_AL(G,vex2);
        ins=(ArcNode*)malloc(sizeof(ArcNode));
        ins->adjvex=loc2;  //存弧头的顶点的序号
        ins->info=weight;  //存该弧所对应的权值
        ins->nextarc=G.vexs[loc1].firstarc->nextarc;  //用头插法将新结点插入
        G.vexs[loc1].firstarc->nextarc=ins;
    }
    return OK;
}
Status CreateDG_AL(ALGraph &G)
{
    int i,loc1,loc2;
    char vex1,vex2;
    ArcNode *ins=NULL;
    printf("请输入该图的顶点个数和弧的个数：");
    scanf("%d %d",&G.vexnum,&G.arcnum);
    getchar();
    printf("请依次输入各顶点：");
    for(i=0;i<G.vexnum;i++)  //顶点向量初始化
    {
        scanf("%c",&G.vexs[i].data);
        G.vexs[i].firstarc=(ArcNode*)malloc(sizeof(ArcNode));
        G.vexs[i].firstarc->nextarc=NULL;
    }
    getchar();
    printf("请输入各弧所对应的两个顶点值：");
    for(i=0;i<G.arcnum;i++)
    {
        scanf("%c %c",&vex1,&vex2);
        getchar();
        loc1=LocateVex_AL(G,vex1);  //只计算弧尾所对应的顶点序号
        loc2=LocateVex_AL(G,vex2);
        ins=(ArcNode*)malloc(sizeof(ArcNode));
        ins->adjvex=loc2;  //存弧头的顶点的序号
        ins->nextarc=G.vexs[loc1].firstarc->nextarc;  //用头插法将新结点插入
        G.vexs[loc1].firstarc->nextarc=ins;
    }
    return OK;
}
Status CreateUDG_AL(ALGraph &G)
{
    int i,loc1,loc2;
    char vex1,vex2;
    ArcNode *ins=NULL;
    printf("请输入该图的顶点个数和边的个数：");
    scanf("%d %d",&G.vexnum,&G.arcnum);
    getchar();
    printf("请依次输入各顶点：");
    for(i=0;i<G.vexnum;i++)  //顶点向量初始化
    {
        scanf("%c",&G.vexs[i].data);
        G.vexs[i].firstarc=(ArcNode*)malloc(sizeof(ArcNode));
        G.vexs[i].firstarc->nextarc=NULL;
    }
    getchar();
    printf("请输入各弧所对应的两个顶点值：");
    for(i=0;i<G.arcnum;i++)
    {
        scanf("%c %c",&vex1,&vex2);   //将两条边均连入链表
        getchar();
        loc1=LocateVex_AL(G,vex1);
        loc2=LocateVex_AL(G,vex2);
        ins=(ArcNode*)malloc(sizeof(ArcNode));
        ins->adjvex=loc2;
        ins->nextarc=G.vexs[loc1].firstarc->nextarc;
        G.vexs[loc1].firstarc->nextarc=ins;
        ins=(ArcNode*)malloc(sizeof(ArcNode));
        ins->adjvex=loc1;
        ins->nextarc=G.vexs[loc2].firstarc->nextarc;
        G.vexs[loc2].firstarc->nextarc=ins;
    }
    return OK;
}
Status CreateALGraph(ALGraph &G)
{
    printf("请输入要创建的图的类型：0，1，2，3分别表示有向图和网、无向图和网\n");
    scanf("%d",&G.kind);
    switch(G.kind)
    {
        case 0: CreateDG_AL(G);return OK;
        case 1: CreateDN_AL(G);return OK;
        case 2: CreateUDG_AL(G);return OK;
        case 3: CreateUDN_AL(G);return OK;
    }
    return OK;
}
