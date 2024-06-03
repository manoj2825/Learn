#include <iostream>

class Base {
public:
      void foo() { std::cout << "Base::foo()" << std::endl; }
};

class Derived : public Base {
public:
    void foo()   { std::cout << "Derived::foo()" << std::endl; }
};

int main() {
    // Base* ptr = new Derived(); // Base class pointer pointing to a Derived object
    // ptr->foo(); // Calls Derived::foo() via vtable
    // ptr->Base::foo(); // Calls Base::foo() explicitly
    // delete ptr;
    //Base * ptr =new Base();
    // Base* ptr = new Derived();
    // ptr->foo();
    // delete ptr;
    Derived o;
    o.foo();
    return 0;
}
