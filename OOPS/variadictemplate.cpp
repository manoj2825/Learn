#include <iostream>
#include "..\Basics\base\manoj.h"

using namespace std;

void print()
{
    cout<<endl;
}

template<typename T, typename... Args>

void print(T first,Args... args)
{
    cout<<first<<" ";
    print(args...);
}
int main() {
    
    print( 1);
    print(3,4,5,5);
    print("asdf","asdf",22);
}