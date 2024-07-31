#include <iostream>
using namespace std;

class Base
{
    public:
        int data;
        Base(int x):data(x)
        { cout<<"Base created"<<endl;}
        virtual void fun()
        {
            cout<<"Base function"<<endl;
        }
};

class derived:public Base
{
    public:
        derived(int y):Base
};

int main() {
    
}