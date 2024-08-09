#include <iostream>
using namespace std;
class Base
{
    public:
        int data;
        Base(int val):data(val){}

        virtual void fun()
        {
            cout<<"derived fun"<<endl;
        }
};

class derived : public Base
{
    public:
    int derived_data;
    derived(int x,int y):Base(y)
    {
        derived_data = x;
        Base* b;
        b=this;
        b->fun();
    }
    void fun() 
    {
        cout<<"derived fun"<<endl;
    }

};
int main() {
    Base* a = new derived(2,5);
}