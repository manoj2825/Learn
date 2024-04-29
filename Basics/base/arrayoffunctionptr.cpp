#include <iostream>
#include "manoj.h"

using namespace std;


int add(int a ,int b)
{
    return a+b;
}

int sub(int a ,int b)
{
    return a-b;
}

int mul(int a ,int b)
{
    return a*b;
}


int main() {
    
    int (*ptr[3]) (int,int);

    ptr[0]=&add;
    ptr[1]=&sub;
    ptr[2]=&mul;

    int a=10,b=5;
    for(int i=0;i<3;i++)
    {
        int result =ptr[i](a,b);
         std::cout << "Result of operation " << i + 1 << ": " << result << std::endl;
    }
    return 0;
}