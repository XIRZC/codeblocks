// 最大m子段和-动态规划求解-m个不相邻的子段和的最大值
//最大子段和即为一个子段的最大值
#include<iostream>
#include<cstring>
#include<climits>
#define max(a,b) ((a)>(b)?(a):(b))

using namespace std;

const int N=1010;  //序列的最长长度
int a[N];  //序列存值数组
int dp[N][N];  //动态规划最优值数组，
//问题转换：dp[i][j]:前j个数中由i个子段组成的最大和，且必须包含第j个数
int main(void)
{
    int n,m;
    cin >>n >>m;
    for(int i=1;i<=n;i++)
        cin >>a[i];
    memset(dp,0,sizeof(dp));
    for(int i=1;i<=m;i++)
    {
        for(int j=i;j<=n;j++)
        {
            dp[i][j]=dp[i][j-1]+a[j];
            for(int k=i-1;k<=j-1;k++)
            {
                dp[i][j]=max(dp[i][j],dp[i-1][k]+a[j]);
            }
        }
    }
    //dp矩阵内容的输出
    for(int i=1;i<=m;i++)
    {
        for(int j=1;j<=n;j++)
        {
           cout <<dp[i][j] <<" ";
        }
        cout <<endl;
    }
    int maxv=dp[m][m];
    for(int i=m+1;i<=n;i++)
    {
        maxv=max(maxv,dp[m][i]);
    }
    cout <<maxv;
    return 0;
}
