#include <iostream>
#include <climits>
#include <vector>

using namespace std;

// void change(int x)
// {
//     x=20;
// }

void change(int* x)
{
    *x=20;//main difference is derefrerencing
}

/*- **Pointers**:
    - Can be reassigned.
    - Can be null.
    - Supports pointer arithmetic.
- **References**:
    - Must be initialized.
    - Cannot be null.
    - No arithmetic.*/

void change(int& x)
{
    x=20;
}


int main()
{
    int x = 10;
    // change(x); //pass  by value
    // cout<<"pass by value : "<<x<<endl;

    change(&x); //pass  by pointer
    cout<<"pass by pointer : "<<x<<endl;

    change(x); //pass  by ref
    cout<<"pass by ref : "<<x<<endl;
}

/*Passing by Pointer:
    Useful when you need to work with dynamic memory or when null pointers are needed to represent optional values.
    Also useful when managing arrays, as pointers can iterate through memory.
Passing by Reference:
    Preferred when you want to avoid unnecessary copying of large data structures (like large objects, vectors, etc.).
    Provides a cleaner, simpler syntax and is often easier to work with.*/