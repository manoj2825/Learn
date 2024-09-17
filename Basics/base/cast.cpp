#include <iostream>
#include <climits>
#include <vector>

using namespace std;

class Base
{
    public:
        virtual void display()
        {
            cout<<"hello"<<endl;
        }
};

class derived final : public Base
{
    public:
        void display() override
        {
            cout<<"hello"<<endl;
        }    
};

int main()
{
    int a =10;
    double b= a;//implicit casting means automatic casting
    cout<<b<<endl;

    //explicit basically user defined casting not automatic

    int x =10;
    double y = static_cast<double>(x);
    cout<<y<<endl;

    Base* baseptr = new derived();

    derived* d = static_cast<derived*>(baseptr);

    d->display(); //this works because baseptr points to derived 

    // what if it point to base itself then

    Base* baseptr2 = new Base();

    derived* d2 = static_cast<derived*>(baseptr2);

    d2->display();// somethimes this will throw undefined behaviour so better ti use dynamic cast when using with virtual

    int c = 10;

    const int* m = const_cast<const int*>(&c);//conver to const type

    
}