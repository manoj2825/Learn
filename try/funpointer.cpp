#include <iostream>
using namespace std;
int add(int x,int y)
{
    return x+y;
}

int sub(int x,int y)
{
    return x-y;
}

int main() {
int (*ptr[1])(int,int);
    ptr[0]=&add;
    ptr[1]=&sub;
    //int ans = ptr(3,4);
    cout<<ans<<endl;
}