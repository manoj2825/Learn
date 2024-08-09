#include <iostream>
#include <climits>
#include <vector>

using namespace std;

void solve(int * x)
{
   int ans =  (*x)*(*x);
}

int* createarray(int size)
{
    int* arr = new int[size];
    return arr;
}

void add(int a,int b)
{   
    int ans = a+b;
    cout<<ans<<endl;
}
int main()
{
    int x = 5;
    solve(&x);

    int* arrptr = createarray(5);

    void (*ptr)(int,int);
    ptr=&add;
    ptr(5,8);

}