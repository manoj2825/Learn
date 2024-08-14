#include <iostream>
using namespace std;

constexpr int square (int x)
{
    return x*x;
}
int main() {
    
 constexpr int ans = square(4);
 cout<<ans<<endl;
}