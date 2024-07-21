#include <iostream>
using namespace std;

void print()
{
    cout<<endl;
}

template<typename T,typename... Args>

void print(T first,Args... args)
{
    cout<<first<<" ";
    print(args...);
}

int main() {
    
}