/* HuffmanTree.cpp -- 英文短文哈夫曼编码以及解码*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<math.h>
typedef char ElemType;
typedef struct{
	ElemType data;
	unsigned int weight;
	unsigned int parent,lChild,rChild;
}HTNode,*HuffmanTree;  //动态分配数组存储赫夫曼树，因不知叶子结点个数
typedef char** HuffmanCode;  //动态分配数组存储赫夫曼编码，因不知叶子节结个数和每个结点的字符长度
void HuffmanCoding(HuffmanTree &HT,HuffmanCode &HC,int *weigh,char *map2char,int n);  //weigh数组各元素要赋初值为0
void select(HuffmanTree &HT,int n,int &min1,int &min2);
void count(int *weigh,int *map2num,char *map2char,int &num);
void passageCoding(HuffmanCode &HC,int *map2num);
void passageUncoding(HuffmanTree &HT,int m);
int main(void)
{
	int weigh[260]={0},num=0,map2num[260]={0};
	char map2char[260]={'\0'};
	HuffmanTree HT;
	HuffmanCode HC;
	count(weigh,map2num,map2char,num);
	HuffmanCoding(HT,HC,weigh,map2char,num);
	passageCoding(HC,map2num);
	passageUncoding(HT,2*num-1);
	return 0;
}
void passageUncoding(HuffmanTree &HT,int m)
{
	char ch;
	int i,key,root;
	FILE *fptR,*fptW;
	fptR=fopen("passageCoding.txt","r");
	fptW=fopen("passageUncoding.txt","w");
	for(i=1;i<=m;i++)
	{
		if(HT[i].parent==0)
			root=i;
	}
	key=root;
	ch=fgetc(fptR);
	while(!feof(fptR))
	{
		key=root;
		while((HT[key].lChild!=0||HT[key].rChild!=0)&&!feof(fptR))
		{
			if(ch=='0')  //左孩子
			{
				key=HT[key].lChild;
			}
			else if(ch=='1') //右孩子
			{
				key=HT[key].rChild;
			}
			else
            {
                fputc(ch,fptW);
            }
			ch=fgetc(fptR);
		}
		fputc(HT[key].data,fptW);
	}
	fclose(fptR);
	fclose(fptW);
}
void passageCoding(HuffmanCode &HC,int *map2num)
{
	FILE *fptR,*fptW;
	char ch;
	fptR=fopen("passage.txt","r");
	fptW=fopen("passageCoding.txt","w");
	ch=fgetc(fptR);
	while(!feof(fptR))
	{
	    if(ch==' '||ch=='\n')
            fputc(ch,fptW);
        else
            fprintf(fptW,"%s",HC[map2num[ch]]);
		ch=fgetc(fptR);
	}
	fclose(fptR);
	fclose(fptW);
}
void count(int *weigh,int *map2num,char *map2char,int &num)
{
    int i=0;
	char ch;
	num=0;
	FILE *fpt=fopen("passage.txt","r");
	ch=fgetc(fpt);
	while(!feof(fpt))
	{
        weigh[ch]++;
		ch=fgetc(fpt);
	}
	for(i=0;i<256;i++)
	{
		if(weigh[i])
		{
			weigh[num]=weigh[i];
			weigh[i]=0;
			map2num[i]=num+1;   //存序号映射
			map2char[num+1]=i;  //存字母映射
			num++;
		}
	}
	fclose(fpt);
}
void select(HuffmanTree &HT,int n,int &min1,int &min2)
{
	int i,temp;
	min1=min2=0;
	for(i=1;i<=n;i++)
	{
		if(HT[i].parent==0)
		{
			if(min1)
			{
				min2=i;
				break;
			}
			min1=i;
		}
	}
	if(HT[min1].weight>HT[min2].weight)
	{
		temp=min1;min1=min2;min2=temp;
	}
	for(i=(min1>min2?min1:min2)+1;i<=n;i++)
	{
		if(HT[i].parent==0)
		{
			if(HT[i].weight<=HT[min1].weight)
			{
				temp=min1;min1=i;min2=temp;
			}
		}
	}
}
void HuffmanCoding(HuffmanTree &HT,HuffmanCode &HC,int *weigh,char *map2char,int n)
{
	int m,i,min1,min2,pre,now,start;
	HTNode * p;
	char *tempCh;  //临时的存储一个叶子节结的字符串的字符数组
	if(n<=1)  return;
	m=2*n-1;  //总结点个数
	HT=(HuffmanTree)malloc(sizeof(HTNode)*(m+1));  //0号单元未用
	for(p=HT+1,i=1;i<=n;i++,p++)   //两部分结点分开初始化
	{
		p->data=map2char[i];  //存叶子结点的字母值
		p->weight=weigh[i-1];
		p->parent=p->lChild=p->rChild=0;
	}
	for(;i<=m;i++,p++)
	{
		p->weight=p->parent=p->lChild=p->rChild=0;
		p->data='\0';
	}
	for(i=n+1;i<=m;i++)
	{
		select(HT,i-1,min1,min2);  //在树中从1到i-1个结点中找到parent为0的最小的两个结点
		HT[min1].parent=HT[min2].parent=i;
		HT[i].lChild=min1;
		HT[i].rChild=min2;
		HT[i].weight=HT[min1].weight+HT[min2].weight;
	}
	//开始根据哈夫曼树从叶子到根逆向求每个字符的哈夫曼编码
	HC=(HuffmanCode)malloc(sizeof(char*)*(n+1));
	tempCh=(char *)malloc(sizeof(char)*(n));
	tempCh[n-1]='\0';
	for(i=1;i<=n;i++)
	{
		start=n-1;
		for(pre=i,now=HT[i].parent;now!=0;pre=now,now=HT[now].parent)
		{
			if(HT[now].lChild==pre)
				tempCh[--start]='0';
			else
				tempCh[--start]='1';
		}
		HC[i]=(char *)malloc(sizeof(char)*(n-start));
		strcpy(HC[i],&tempCh[start]);  //从第start个字符起进行赋值
	}
	free(tempCh);
}
