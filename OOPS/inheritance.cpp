#include <iostream>
#include "..\Basics\base\manoj.h"

// C++ Implementation to show that a derived class
// doesn’t inherit access to private data members.
// However, it does inherit a full parent object.
class A {
public:
    int x;
 
protected:
    int y;
 
private:
    int z;
};
 
class B : public A {
    // x is public
    // y is protected
    // z is not accessible from B
};
 
class C : protected A {
    // x is protected
    // y is protected
    // z is not accessible from C
};
 
class D : private A // 'private' is default for classes
{
    // x is private
    // y is private
    // z is not accessible from D
};
using namespace std;

int main() {
    
   B b;
   C c;
   D d;

  // d.x=5;


    
    return 0;
}