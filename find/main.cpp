#include<stdio.h>
#include<stdlib.h>
#define TRUE 1
#define FALSE 0
#define SUCCESS 1
#define UNSUCCESS 0
#define DUPLICATE -1
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2
#define HASH_SIZE 100
#define LIST_INIT_SIZE 100
#define LISTINCREMENT 10
typedef int Status;
typedef int ElemType;
typedef struct{
	ElemType *elem;  //顺序表的起始地址
	int length;
	int listSize;
}SqList;
typedef struct{
    ElemType *elem;  //数据元素基址
    int count;   //当前数据元素个数
    int SizeIndex;
}HashTable;
typedef int TElemType;
typedef struct BiTNode{  //二叉树数据表示
	TElemType data;
	struct BiTNode *lChild,*rChild;  //左右孩子指针
}BiTNode,*BiTree;
Status CreateBiTree(BiTree &T);
Status InOrderTraverse(BiTree T,Status(* visit)(TElemType elem));
Status TElemOutput(TElemType elem);
Status InitList(SqList * L);
Status ListInsert(SqList * L,ElemType add);
Status ListDelete(SqList * L,int num,ElemType *delPt);
Status ListCreate(SqList * L);
Status GetList(SqList * L);
Status compare(ElemType elem1,ElemType elem2);
int SqSearch(SqList *L,int key);
int BinarySearch(SqList *L,int key);
//二叉排序树的查找和逐个插入
Status SearchBST(BiTree T,int key,BiTree f,BiTree &p);
Status InsertBST(BiTree &T,ElemType elem);
Status CreateBST(BiTree &T);
//hash表的创建、插入和查找以及开放地址法线性探测再散列去解决冲突
Status InitHash(HashTable &H);
int Hash(int key);
void collision(int &pos,int &cnt);
Status SearchHash(HashTable H,ElemType key,int &pos,int &cnt);
Status CreateHash(HashTable &H);
Status InsertHash(HashTable &H,ElemType elem);
/*
二叉排序树样例输入：
45 24 53 45 12 24 90 q
*/
int main(void)
{
    int key,num,pos=0,cnt=0;
    BiTree T=NULL,p=NULL;
    HashTable H;
    /*  顺序表的顺序查找以及二分查找
    SqList L;
    InitList(&L);
    ListCreate(&L);
    printf("顺序查找%d 二分查找%d",SqSearch(&L,key),BinarySearch(&L,key));
    */
    /*  二叉排序树的创建、插入、查找
    CreateBST(T);
    printf("二叉排序树中序遍历序列为：\n");
    InOrderTraverse(T,TElemOutput);
    putchar('\n');
    printf("请输入要插入的关键字，按q键离开输入：\n");
    while(scanf("%d",&num))
        InsertBST(T,num);
    getchar();
    printf("插入后二叉排序树中序遍历序列为：\n");
    InOrderTraverse(T,TElemOutput);
    putchar('\n');
    printf("请输入要查找的关键字，按q键离开输入：");
    while(scanf("%d",&key))
        if(SearchBST(T,key,NULL,p))
            printf("当前序列中存在该关键字\n");
        else
            printf("当前序列中不存在该关键字\n");
    */
    //哈希表的创建、插入以及查找
    InitHash(H);
    CreateHash(H);
    printf("请输入要查找的关键字，按q键离开输入：");
    while(scanf("%d",&key))
        if(SearchHash(H,key,pos,cnt))
            printf("当前序列中存在该关键字且位于地址%d\n",pos);
        else
            printf("当前序列中不存在该关键字\n");
    return 0;
}
Status InitHash(HashTable &H)
{
    int i;
    H.elem=(ElemType*)malloc(HASH_SIZE*sizeof(ElemType));
    if(!H.elem) return ERROR;
    H.count=0;
    H.SizeIndex=HASH_SIZE;
    for(i=0;i<H.SizeIndex;i++)
    {
        H.elem[i]=0;
    }
    return OK;
}
Status SearchHash(HashTable H,ElemType key,int &pos,int &cnt)
{
    pos=Hash(key);
    while(H.elem[pos]!=0&&H.elem[pos]!=key)
        collision(pos,++cnt);
    if(key==H.elem[pos])
        return SUCCESS;
    else
        return UNSUCCESS;
}
void collision(int &pos,int &cnt)
{
    pos++;
}
int Hash(ElemType key)
{
    return key%HASH_SIZE;
}
Status InsertHash(HashTable &H,ElemType elem)
{
    int pos=0,cnt=0;
    if(SearchHash(H,elem,pos,cnt))
        return DUPLICATE;
    else
    {
        H.elem[pos]=elem;
        H.count++;
        return OK;
    }
}
Status CreateHash(HashTable &H)
{
    int num;
    printf("请依次输入查找序列，q键离开输入来完成Hash表的创建：");
    while(scanf("%d",&num))
        InsertHash(H,num);
    getchar();
    return OK;
}
Status CreateBST(BiTree &T)
{
    int num;
    printf("请依次输入查找序列，q键离开输入来完成二叉排序树的创建：");
    while(scanf("%d",&num))
        InsertBST(T,num);
    getchar();
    return OK;
}
Status SearchBST(BiTree T,int key,BiTree f,BiTree &p)
{  //用T代表当前子树的指针，f代表当前子树指针的双亲指针，
    //若查找成功则用p返回其结点的地址，否则用p返回查找路径上访问的最后一个结点，f初始值为空
    if(!T)  //该子树为空则必查找不成功
    {
        p=f;
        return FALSE;
    }
    else if(key==T->data)  //查找成功
    {
        p=T;
        return TRUE;
    }
    else if(key<T->data)   //小于，则继续访问左子树
        return SearchBST(T->lChild,key,T,p);
    else    //大于，则继续访问右子树
        return SearchBST(T->rChild,key,T,p);
}
Status InsertBST(BiTree &T,ElemType elem)
{
    BiTNode *p,*f=NULL,*ins=NULL;
    if(!SearchBST(T,elem,f,p))
    {
        ins=(BiTNode*)malloc(sizeof(BiTNode));
        ins->data=elem;
        ins->lChild=ins->rChild=NULL;
        if(!p)  //此时该子树为空
        {
            T=ins;  //直接插入
        }
        else if(p->data>ins->data)  //小于则在左子树上插
            p->lChild=ins;
        else
            p->rChild=ins;
        return TRUE;
    }
    else return FALSE;
}
int SqSearch(SqList *L,int key)
{
    int i;
    for(i=0;i<L->length;i++)
    {
        if(L->elem[i]==key)
            return i;
    }
    return -1;
}
int BinarySearch(SqList *L,int key)
{
    int left=0,right=L->length,mid;
    while(left<=right)
    {
        mid=(left+right)/2;
        if(L->elem[mid]>key)
            right=mid-1;
        else if(L->elem[mid]<key)
            left=mid+1;
        else
            return mid;
    }
    return -1;
}
Status InitList(SqList * L)  //初始化顺序表，为顺序表分配内存空间及生成基地址
{
	L->elem=(ElemType*)malloc(LIST_INIT_SIZE*sizeof(ElemType));
	if(!L->elem)
		return ERROR;
	L->length=0;
	L->listSize=LIST_INIT_SIZE;
	return OK;
}
Status ListInsert(SqList * L,ElemType add)
{
	ElemType *newBase;
	if(L->length>L->listSize)
	{
		newBase=(ElemType*)realloc(L->elem,(L->listSize+LISTINCREMENT)*sizeof(ElemType));
		if(!newBase)  return OVERFLOW;
		L->elem=newBase;
		L->listSize+=LISTINCREMENT;
	}
	L->elem[L->length]=add;
	L->length++;
	return OK;
}
Status ListDelete(SqList * L,int num,ElemType *delPt)
{
	int i;
	if(L->length==0||num<1||num>L->length)
		return ERROR;
	*delPt=L->elem[num-1];
	for(i=num-1;i<L->length;i++)
	{
		L->elem[i]=L->elem[i+1];
	}
	L->length--;
	return OK;
}
Status ListCreate(SqList * L)  //根据插入函数循环生成一个有序顺序表
{
	ElemType elem;
	printf("创建顺序表中,q离开输入：\n");
	while(scanf("%d",&elem))
	{
		ListInsert(L,elem);
	}
	return OK;
}
Status compare(ElemType elem1,ElemType elem2)
{
	return elem1>elem2;
}
Status GetList(SqList * L)
{
	int i;
	if(L->length==0)  return ERROR;
	for(i=0;i<L->length;i++)
	{
		printf("%d\t",L->elem[i]);
	}
	printf("\n");
	return OK;
}
Status TElemOutput(TElemType elem)
{
	if(!printf("%d\t",elem)) return ERROR;
	return OK;
}
Status CreateBiTree(BiTree &T)
{
	char ch;
	scanf("%c",&ch);
	if(ch==' ')
	{
		T=NULL;
		return 0;
	}
	else
	{
		T=(BiTNode *)malloc(sizeof(BiTNode));
		T->data=ch;
		CreateBiTree(T->lChild);
		CreateBiTree(T->rChild);
	}
	return OK;
}
Status InOrderTraverse(BiTree T,Status(* visit)(TElemType elem))  //二叉树中序递归遍历
{
	if(!T)  return 0;
	else
	{
		InOrderTraverse(T->lChild,visit);
		visit(T->data);
		InOrderTraverse(T->rChild,visit);
		return OK;
	}
}
