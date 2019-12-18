#include "MGraph.h"
int visited[MAX_VERTEX_NUM]={0};  //顶点访问标志数组
void DFSTraverse(MGraph G,Status(*Visit)(VertexType vex))
{
    int i;
    for(i=0;i<G.vexnum;i++)
    {
        if(!visited[i])
            DFS(G,i,Visit);
    }
}
void DFS(MGraph G,int vex,Status(*Visit)(VertexType vex))
{
    int i=vex,j;
    visited[i]=1;
    Visit(G.vexs[i]);
    for(j=0;j<G.vexnum;j++)
    {
        if((((G.kind==0||G.kind==2)&&G.arcs[i][j].adj)||((G.kind==1||G.kind==3)&&G.arcs[i][j].adj!=INFINITY))&&!visited[j])
            DFS(G,j,Visit);
    }
}
int LocateVex(MGraph &G,VertexType vex)
{
    int i;
    for(i=0;i<G.vexnum;i++)
    {
        if(G.vexs[i]==vex)
            return i;
    }
    return -1;
}
Status CreateUDN_M(MGraph &G)  //创建无向网
{
    int i,j,weight,loc1,loc2;
    char vex1,vex2;
    printf("请输入顶点的个数和边的个数：");
    scanf("%d %d",&G.vexnum,&G.arcnum);
    getchar();
    printf("请依次输入各顶点：");
    for(i=0;i<G.vexnum;i++)
        scanf("%c",&G.vexs[i]);
    getchar();
    for(i=0;i<G.vexnum;i++)
    {
        for(j=0;j<G.vexnum;j++)
        {
            G.arcs[i][j].adj=INFINITY;
            G.arcs[i][j].info=NULL;
        }
    }
    printf("请依次输入各条边所对应的两顶点值和权值：");
    for(i=0;i<G.arcnum;i++)
    {
        scanf("%c %c %d",&vex1,&vex2,&weight);
        getchar();
        loc1=LocateVex(G,vex1);
        loc2=LocateVex(G,vex2);
        G.arcs[loc1][loc2].adj=weight;
        G.arcs[loc2][loc1]=G.arcs[loc1][loc2];
    }
    return OK;
}
Status CreateDN_M(MGraph &G)   //创建有向网
{
    int i,j,weight,loc1,loc2;
    char vex1,vex2;
    printf("请输入顶点的个数和边的个数：");
    scanf("%d %d",&G.vexnum,&G.arcnum);
    printf("请依次输入各顶点：");
    getchar();
    for(i=0;i<G.vexnum;i++)
        scanf("%c",&G.vexs[i]);
    for(i=0;i<G.vexnum;i++)
    {
        for(j=0;j<G.vexnum;j++)
        {
            G.arcs[i][j].adj=INFINITY;
            G.arcs[i][j].info=NULL;
        }
    }
    getchar();
    printf("请依次输入各条弧所对应的顶点值和权值：");
    for(i=0;i<G.arcnum;i++)
    {
        scanf("%c %c %d",&vex1,&vex2,&weight);
        getchar();
        loc1=LocateVex(G,vex1);
        loc2=LocateVex(G,vex2);
        G.arcs[loc1][loc2].adj=weight;
    }
    return OK;
}
Status CreateUDG_M(MGraph &G)
{
    int i,j,loc1,loc2;
    char vex1,vex2;
    printf("请输入顶点的个数和弧的个数：");
    scanf("%d %d",&G.vexnum,&G.arcnum);
    printf("请依次输入各顶点：");
    getchar();
    for(i=0;i<G.vexnum;i++)
        scanf("%c",&G.vexs[i]);
    for(i=0;i<G.vexnum;i++)
    {
        for(j=0;j<G.vexnum;j++)
        {
            G.arcs[i][j].adj=0;
            G.arcs[i][j].info=NULL;
        }
    }
    getchar();
    printf("请依次输入各条弧的两个顶点值：");
    for(i=0;i<G.arcnum;i++)
    {
        scanf("%c %c",&vex1,&vex2);
        getchar();
        loc1=LocateVex(G,vex1);
        loc2=LocateVex(G,vex2);
        G.arcs[loc1][loc2].adj=1;
        G.arcs[loc2][loc1]=G.arcs[loc1][loc2];
    }
    return OK;
}
Status CreateDG_M(MGraph &G)
{
    int i,j,loc1,loc2;
    char vex1,vex2;
    printf("请输入顶点的个数和弧的个数：");
    scanf("%d %d",&G.vexnum,&G.arcnum);
    getchar();
    printf("请依次输入各顶点：");
    for(i=0;i<G.vexnum;i++)
        scanf("%c",&G.vexs[i]);
    for(i=0;i<G.vexnum;i++)
    {
        for(j=0;j<G.vexnum;j++)
        {
            G.arcs[i][j].adj=0;
            G.arcs[i][j].info=NULL;
        }
    }
    getchar();
    printf("请依次输入各条弧所对应的顶点值：");
    for(i=0;i<G.arcnum;i++)
    {
        scanf("%c %c",&vex1,&vex2);
        getchar();
        loc1=LocateVex(G,vex1);
        loc2=LocateVex(G,vex2);
        G.arcs[loc1][loc2].adj=1;
    }
    return OK;
}
Status CreateMGraph(MGraph &G)
{
    printf("请输入要创建的图的类型：0，1，2，3分别表示有向图和网、无向图和网\n");
    scanf("%d",&G.kind);
    switch(G.kind)
    {
        case 0: CreateDG_M(G);return OK;
        case 1: CreateDN_M(G);return OK;
        case 2: CreateUDG_M(G);return OK;
        case 3: CreateUDN_M(G);return OK;
    }
    return OK;
}

