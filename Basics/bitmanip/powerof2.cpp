#include <iostream>
#include "manoj.h"

using namespace std;

bool isPowerOfTwoRecursive(int n)
{	
    // Write your code here.
    if(n==1)
    {
        return true;
    }
    if(n&1) return false;
    
    return isPowerOfTwoRecursive(n/2);
}

int main() {
    
    bool ans=isPowerOfTwoRecursive(16);

    if(ans)
        cout<<"true"<<endl;
    else
        cout<<"false"<<endl;
}