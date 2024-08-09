#include <iostream>
using namespace std;

class Base {
public:
    void display() {
        cout << "Base class display method." << endl;
    }
};

class Intermediate1 : virtual public Base {
public:
    void show1() {
        cout << "Intermediate1 class show1 method." << endl;
    }
};

class Intermediate2 : virtual public Base {
public:
    void show2() {
        cout << "Intermediate2 class show2 method." << endl;
    }
};

class Derived : public Intermediate1, public Intermediate2 {
public:
    void reveal() {
        cout << "Derived class reveal method." << endl;
    }
};

int main() {
    Derived d;
    d.show1();   // Intermediate1 class method
    d.show2();   // Intermediate2 class method
    d.reveal();  // Derived class method
   // d.Intermediate2::display();
    d.display(); // No ambiguity, single Base instance
    return 0;
}