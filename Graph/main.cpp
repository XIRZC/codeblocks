#include "base.h"
#include "MGraph.h"
#include "ALGraph.h"
void MiniSpanTree_PRIM(MGraph G,VertexType vex);  //利用prim算法去求一个图的最小生成树(MST)
int minEdge(Edge *closedge,MGraph G);
Status TopologicalSort(ALGraph G);
void FindInDegree(int *indegree,ALGraph G);
Status TopologicalOrder(ALGraph G,SqStack &T,int *ves);
Status CriticalPath(ALGraph &G);
void ShortestPath_DIJ(MGraph &G,VertexType vex0,int P[][MAX_VERTEX_NUM],int D[]);
/*样例输入：
8 9
ABCDEFGH
图输入：AB AC BD BE DH EH CF CG FG
网输入：AB 1 AC 2 BD 1 BE 3 DH 3 EH 2 CF 1 CG 2 FG 3
输出： 邻接矩阵存储法深度优先遍历：ABDHECFG
邻接表存储法深度优先遍历(头插法，先输入的边后输出邻接点）：ACGFBEHD

最小生成树输入： （无向网）
6 10
ABCDEF
AB 6 AC 1 AD 5 BC 5 CD 5 BE 3 CE 6 CF 4 EF 6 DF 2
输出：AC CF FD CB BE,从不同起始点得出的最小生成树是相同的

拓扑排序输入：  （有向无环图）
6 8
ABCDEF
AB AC AD CD EB EF BF CF
输出：E A B C D F
有环图： AB AC DA CD EB EF BF CF

关键路径输入：  （有向无环网）
6 8
ABCDEF
AB 3 AC 2 BD 2 BE 3 CD 4 CF 3 DF 2 EF 1
输出：关键路径：A->C->D->F

9 11
123456789
12 6 13 4 14 5 25 1 35 1 46 2 57 9 58 7 68 4 79 2 89 4
输出：关键活动： 12 25 57 58 79 89

最短路径输入：  （有向网）
6 8
012345
05 100 04 30 02 10 12 5 23 50 43 20 35 10 45 60
输出：0 1000000 10  50  30  60
*/
int main(void)
{
    int P[MAX_VERTEX_NUM][MAX_VERTEX_NUM]={0},D[MAX_VERTEX_NUM]={0},i;
    MGraph MG;
    CreateMGraph(MG);
    //ALGraph ALG;
    //CreateALGraph(ALG);
    // 邻接矩阵深度优先遍历
    //DFSTraverse(MG,VexOutput);
    // 邻接表深度优先遍历
    //DFSTraverse_AL(ALG,VexOutput);
    //邻接矩阵上实现最小生成树的求法,创建图为无向网
    //MiniSpanTree_PRIM(MG,MG.vexs[1]);
    //邻接表上实现拓扑排序
    //if(!TopologicalSort(ALG))
    //    printf("该有向图不为无环图");
    //if(!CriticalPath(ALG))
    //    printf("该有向图不为无环图\n");
    //最短路径
    ShortestPath_DIJ(MG,MG.vexs[0],P,D);
    for(i=0;i<MG.vexnum;i++)
    {
        printf("%d\t",D[i]);
    }
    return 0;
}
void ShortestPath_DIJ(MGraph &G,VertexType vex0,int P[][MAX_VERTEX_NUM],int D[])
{
    int final[MAX_VERTEX_NUM]={0};
    int i,j,v,min;
    int v0=LocateVex(G,vex0);
    for(v=0;v<G.vexnum;v++)
    {
        D[v]=G.arcs[v0][v].adj;
        if(D[v]<INFINITY)
        {
            P[v][v0]=1;
            P[v][v]=1;
        }
    }
    D[v0]=0;
    final[v0]=1;  //v0顶点属于S集合
    for(i=1;i<G.vexnum;i++)
    {
        min=INFINITY;
        for(j=0;j<G.vexnum;j++)
        {
            if(!final[j])
            {
                if(D[j]<min)
                {
                    min=D[j];
                    v=j;
                }
            }
        }
        final[v]=1;
        for(j=0;j<G.vexnum;j++)
        {
            if(!final[j]&&(D[j]>G.arcs[v][j].adj+min))
            {
                D[j]=G.arcs[v][j].adj+min;
                P[j][j]=1;
            }
        }
    }
}
Status CriticalPath(ALGraph &G)
{
    int ve[MAX_VERTEX_NUM]={0},vl[MAX_VERTEX_NUM]={0},i,k;
    char tag[30];
    ArcNode *temp;
    SqStack T;
    if(!TopologicalOrder(G,T,ve))
        return ERROR;
    vl[G.vexnum-1]=ve[G.vexnum-1];  //逆拓扑有序序列从n-1开始递推
    while(!StackEmpty(T))
    {
        Pop(T,i);
        for(temp=G.vexs[i].firstarc->nextarc;temp;temp=temp->nextarc)
        {
            if(!temp)  break;
            if(vl[i]==0)  vl[i]=vl[temp->adjvex]-*(temp->info);  //保证vl[i]的值是vl[temp->adjvex]-*(temp->info)的最小值
            if(vl[temp->adjvex]-*(temp->info)<vl[i])
                vl[i]=vl[temp->adjvex]-*(temp->info);
        }
    }
    for(i=0;i<G.vexnum;i++)
    {
        for(temp=G.vexs[i].firstarc->nextarc;temp;temp=temp->nextarc)
        {
            if(!temp)  break;
            k=temp->adjvex;
            if(ve[i]==vl[k]-*(temp->info))
            {
                strcpy(tag,"该活动为关键活动");
            }
            else
                strcpy(tag,"  ");
            printf("活动%c->%c 耗时%d 最早时间%d 最晚时间%d %s\n",G.vexs[i].data,G.vexs[k].data,*(temp->info),ve[i],vl[k]-*(temp->info),tag);
        }
    }
    return OK;
}
Status TopologicalOrder(ALGraph G,SqStack &T,int *ve)
{
    int indegree[MAX_VERTEX_NUM]={0},i,count=0;
    ArcNode *temp;
    SqStack S;
    InitStack(S);
    InitStack(T);
    FindInDegree(indegree,G);
    for(i=0;i<G.vexnum;i++)
    {
        if(!indegree[i])
            Push(S,i);
    }
    while(!StackEmpty(S))
    {
        Pop(S,i);
        Push(T,i);
        count++;
        for(temp=G.vexs[i].firstarc->nextarc;temp;temp=temp->nextarc)
        {
            if(!temp)  break;
            if(!--indegree[temp->adjvex])
            {
                Push(S,temp->adjvex);
            }
            if(ve[i]+*(temp->info)>ve[temp->adjvex])
                ve[temp->adjvex]=ve[i]+*(temp->info);
        }
    }
    if(count<G.vexnum)
        return ERROR;
    else
        return OK;
}
Status TopologicalSort(ALGraph G)
{
    int indegree[MAX_VERTEX_NUM]={0},i,count=0;
    ArcNode *temp;
    SqStack S;
    InitStack(S);
    FindInDegree(indegree,G);
    for(i=0;i<G.vexnum;i++)
    {
        if(!indegree[i])
            Push(S,i);
    }
    while(!StackEmpty(S))
    {
        Pop(S,i);
        printf("%c  ",G.vexs[i].data);
        count++;
        for(temp=G.vexs[i].firstarc->nextarc;temp;temp=temp->nextarc)
        {
            if(!temp)  break;
            if(!--indegree[temp->adjvex])
            {
                Push(S,temp->adjvex);
            }
        }
    }
    if(count<G.vexnum)
        return ERROR;
    else
        return OK;
}
void FindInDegree(int *indegree,ALGraph G)
{
    int i;
    ArcNode *temp;
    for(i=0;i<G.vexnum;i++)
    {
        for(temp=G.vexs[i].firstarc->nextarc;temp;temp=temp->nextarc)
        {
            if(!temp)  break;
            indegree[temp->adjvex]++;
        }
    }
}
void MiniSpanTree_PRIM(MGraph G,VertexType vex)
{
    int k,i,j,fix;
    Edge closedge[MAX_VERTEX_NUM];
    fix=k=LocateVex(G,vex);
    for(i=0;i<G.vexnum;i++)  //辅助数组初始化
    {
        if(i!=k)
        {
            closedge[i].adjvex=vex;   //依附到U集合中的顶点值
            closedge[i].lowcost=G.arcs[k][i].adj;
        }
    }
    closedge[k].lowcost=0;
    closedge[k].adjvex=vex;
    for(i=0;i<G.vexnum;i++)  //开始依次找与第k个结点依附的权值最小的边并开始并入集合U中
    {
        if(i!=fix)
        {
            k=minEdge(closedge,G);   //找到第k个结点依附的权值最小的边所对应的邻接点序号
            printf("%c %c\n",closedge[k].adjvex,G.vexs[k]);
            closedge[k].lowcost=0;   //并入集合U中
            for(j=0;j<G.vexnum;j++)  //更新closedge的值,从U到V-U中的相对应的V-U中的顶点最小权值序偶可能改变
            {
                if(G.arcs[k][j].adj<closedge[j].lowcost)
                {
                    closedge[j].lowcost=G.arcs[k][j].adj;
                    closedge[j].adjvex=G.vexs[k];
                }
            }
        }
    }
}
int minEdge(Edge *closedge,MGraph G)
{
    int i,minVex;
    for(i=0;i<G.vexnum;i++)
    {
        if(closedge[i].lowcost!=0)  //找一个符合条件即lowcost不为0的minVex值
        {
            minVex=i;
            break;
        }
    }
    for(i=minVex+1;i<G.vexnum;i++)
    {
        if(minVex!=i&&closedge[i].lowcost!=0&&closedge[minVex].lowcost>closedge[i].lowcost)
            minVex=i;
    }
    return minVex;
}
Status VexOutput(VertexType vex)
{
    printf("%c",vex);
    return OK;
}
Status InitStack(SqStack &S)
{
	S.base=(SElemType*)malloc(STACK_INIT_SIZE*sizeof(SElemType));
	if(!S.base)  exit(ERROR);
	S.stackSize=STACK_INIT_SIZE;
	S.top=S.base;
	return OK;
}
Status Pop(SqStack &S,SElemType &del)
{
	if(S.top==S.base)  exit(ERROR);
	del=*--S.top;
	return OK;
}
Status Push(SqStack &S,SElemType add)
{
	if(S.top-S.base>=S.stackSize)
	{
		S.base=(SElemType *)realloc(S.base,(S.stackSize+STACKINCREMENT)*sizeof(SElemType));
		if(!S.base)  exit(OVERFLOW);
		S.stackSize+=STACKINCREMENT;
		S.top=S.base+STACKINCREMENT;
	}
	*S.top++=add;
	return OK;
}
Status StackEmpty(SqStack &S)
{
	return S.top==S.base;
}
