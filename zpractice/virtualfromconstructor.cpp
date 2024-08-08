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
        derived(int y,int x):Base(x){
            Base *b;
            b=this;
            b->fun();
        }
        void fun()
        {
            cout<<"Derived Function"<<endl;
        }
};

int main() {
    
    Base* obj =new derived(5,2);
}