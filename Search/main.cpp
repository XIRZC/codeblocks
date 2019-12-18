#include<stdio.h>
#include<stdlib.h>
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2
#define LIST_INIT_SIZE 100
#define LISTINCREMENT 10
typedef int Status;
typedef int ElemType;
typedef struct{
	ElemType *elem;  //˳������ʼ��ַ
	int length;
	int listSize;
}SqList;
Status InitList(SqList * L);
Status ListInsert(SqList * L,ElemType add);
Status ListDelete(SqList * L,int num,ElemType *delPt);
Status ListCreate(SqList * L);
Status GetList(SqList * L);
int SqSearch(SqList *L,int key);
int BinarySearch(SqList *L,int key);
int main(void)
{
    int key,result;
    SqList L;
    InitList(&L);
    ListCreate(&L);
    getchar();  //����q�ַ�
    printf("������Ҫ���ҵĹؼ��֣�");
    scanf("%d",&key);
    if((result=SqSearch(&L,key))!=-1)
        printf("����λ��Ϊ%d��\n",result+1);
    else
        printf("δ���ҵ�������\n");
    if((result=BinarySearch(&L,key))!=-1)
        printf("����λ��Ϊ%d��\n",result+1);
    else
        printf("δ���ҵ�������\n");
    return 0;
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
Status InitList(SqList * L)  //��ʼ��˳���Ϊ˳�������ڴ�ռ估���ɻ���ַ
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
Status ListCreate(SqList * L)  //���ݲ��뺯��ѭ������һ������˳���
{
	ElemType elem;
	printf("����˳�����,q�뿪���룺\n");
	while(scanf("%d",&elem))
	{
		ListInsert(L,elem);
	}
	return OK;
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
