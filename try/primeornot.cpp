#include <iostream>
#include <string.h>
#include <algorithm>
#include <vector>
using namespace std;

bool isprime(int n)
{
    if(n<=1) return false;
    if(n<=3)
    {   
        return true;
    } 
    if (n % 2 == 0 || n % 3 == 0) return false;// for effciency

    for(int i=2;i<n/2;++i)
    {
        if(n%i==0)
        {
            return false;
        }
    }
    return true;
}


int main() {

    int n=100;
     vector<int> primes;
    cout<<(isprime(n)==1?"Yes":"No")<<endl;
   
    cout<<"Printing prime numbers from 2 to N : "<<endl;
    for(int i=0;i<=n;i++)
    {
        if(isprime(i)) primes.push_back(i);
    }
    for(int prime :primes)
    {
        cout<<prime<<" ";
    }
}