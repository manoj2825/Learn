#include <iostream> 
using namespace std;

int main() {
    int x = 10;         // x is an lvalue
    cout<<x<<endl;
    int& ref = x;       // ref is an lvalue reference, binds to x
    cout<<x<<endl;
    ref = 20;           // Modifies the value of x
    cout<<x<<endl;
    int&& rref = 42;    // rref is an rvalue reference, binds to the temporary integer 42
    cout<<rref<<endl;
    int&& result = x + 5; // result is an rvalue reference, binds to the temporary result of the expression x + 5
    cout<<rref<<endl;
    cout<<result<<endl;

    return 0;
}