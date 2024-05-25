#include <iostream>
#include "..\Basics\base\manoj.h"

using namespace std;

template<typename T>

T sum(T value)
{
    return value;
}

template<typename T, typename... Args>

T sum(T first, Args... args)
{
    return first + sum(args...);
}

int main() {
    
    cout<<sum(1)<<endl;
    cout<<sum(1,3)<<endl;
    cout<<sum(1,3,2,5)<<endl;
}