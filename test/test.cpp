/*
#include<iostream>
#include<cstring>
#define max(a,b) ((a)>(b)?(a):(b))


using namespace std;

const int N=110;  //最多有的物品数
const int M=10005;  //小明最多有多少钱
int c[N],w[N];  //c代表物品花费，w代表物品期待价值
int dp[N][M];
int main(void)
{
    int t,n,m;
    cin >>t;
    for(int k=1;k<=t;k++)
	{
        cin >>m >>n;
        for(int i=1;i<=n;i++)
			cin >>c[i];
		for(int i=1;i<=n;i++)
			cin >>w[i];
		memset(dp,0,sizeof(dp));
        for(int i=1;i<=n;i++)
		{
			for(int j=1;j<=m;j++)
			{
				if(j>=c[i])
					dp[i][j]=max(dp[i-1][j],dp[i-1][j-c[i]]+w[i]);
				else
					dp[i][j]=dp[i-1][j];
			}
		}
		cout <<"case #" <<k <<": " <<dp[n][m] <<endl;
	}
    return 0;
} */
