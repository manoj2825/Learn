#include <iostream>
#include <climits>
#include <vector>

using namespace std;

int usingdp(int n)
{
    if(n ==0 || n==1) return 1;

    vector<int> dp(n+1);
    dp[0]=1;
    dp[1]=1;

    for(int i =2; i<=n;++i)
    {
        dp[i] = dp[i-1]+dp[i-2];
    }

    return dp[n];
}

int usingrecursion(int n)
{
    if(n ==0 || n==1)
    {
        return 1;
    }

    return usingrecursion(n-1)+usingrecursion(n-2);
}

int main()
{
    int n = 1e5+1;
    cout<<"Using DP : "<<usingdp(n)<<endl;
    cout<<"Using Recursion : "<<usingrecursion(n)<<endl;
}