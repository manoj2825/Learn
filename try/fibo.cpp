#include <iostream>
#include <string.h>
#include <algorithm>
using namespace std;

void fibo(int n)
{
    int first = 0;
    int second  =  1;
    int next;

    for(int i=0; i<n;i++)
    {
        if(i<=1)
            next =i;
        else
            next = first+second;
            first = second;
            second = next;
        cout<<next<<" ";
    }
}

int main() {

    int n=10;
    cout<<"Printing ... : "<<endl;
    fibo(n);
   
}