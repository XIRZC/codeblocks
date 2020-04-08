// ���m�Ӷκ�-��̬�滮���-m�������ڵ��Ӷκ͵����ֵ
//����Ӷκͼ�Ϊһ���Ӷε����ֵ
#include<iostream>
#include<cstring>
#include<climits>
#define max(a,b) ((a)>(b)?(a):(b))

using namespace std;

const int N=1010;  //���е������
int a[N];  //���д�ֵ����
int dp[N][N];  //��̬�滮����ֵ���飬
//����ת����dp[i][j]:ǰj��������i���Ӷ���ɵ����ͣ��ұ��������j����
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
    //dp�������ݵ����
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
