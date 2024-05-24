#include <iostream>
#include "..\Basics\base\manoj.h"

using namespace std;

class  base {
    public:
        base()
        {
            cout<<"base created"<<endl;
        }
            void display() {
        cout << "Base class display method." << endl;
    }
};

class  midone:public base {
    public:
        midone()
        {
            cout<<"midone created"<<endl;
        }
    void show1() {
        cout << "Intermediate1 class show1 method." << endl;
    }
};

class  midtwo:public base {
    public:
        midtwo()
        {
            cout<<"midtwo created"<<endl;
        }

            void show2() {
        cout << "Intermediate1 class show2 method." << endl;
    }
};

class derived : public midone ,public midtwo
{
        public: 
        derived():midone(),midtwo() //this is how you create a constructor in a multiple derived class
        {
            cout<<"derived created"<<endl;
        }

            void reveal() {
        cout << "Derived class reveal method." << endl;
    }
};

int main() {
    
    derived d;

    d.show1();
    d.show2();
    d.reveal();
    d.midone::display(); // to solce daimond prblem or No ambiguity
    return 0;
}