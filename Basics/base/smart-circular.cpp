#include <iostream>
#include <climits>
#include <vector>
#include <memory>

using namespace std;

class B;

class A
{
    public:
        shared_ptr<B> ptr;
        ~A(){cout<<"A destroyed";}
};

class B
{
    public:
        shared_ptr<A> ptr;
        ~B(){cout<<"B destroyed";}
};

int main()
{
    shared_ptr<A> a = make_shared<A>(); // Create shared_ptr for A
    shared_ptr<B> b = make_shared<B>(); // Create shared_ptr for B

    cout << "Initial reference count for a: " << a.use_count() << endl; // Should be 1
    cout << "Initial reference count for b: " << b.use_count() << endl; // Should be 1

    a->ptr = b;  // A points to B
    cout << "After a points to b:" << endl;
    cout << "Reference count for a: " << a.use_count() << endl; // Should be 1
    cout << "Reference count for b: " << b.use_count() << endl; // Should be 2 (a->ptr + b)

    b->ptr = a;  // B points back to A, creating a cyclic reference
    cout << "After b points to a (cyclic reference):" << endl;
    cout << "Reference count for a: " << a.use_count() << endl; // Should be 2 (b->ptr + a)
    cout << "Reference count for b: " << b.use_count() << endl; // Should still be 2

    return 0;

}