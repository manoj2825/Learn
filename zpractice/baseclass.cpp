#include <iostream>
using namespace std;

class Base
{
    public:
        int *data;

        //default constructor
        Base(int val)
        {
            data =new int;
            *data =val;
        }

        //copy
        Base(const Base& other)
        {
            data =new int;
            *data =*(other.data);
        }

        //move
        Base(Base&& other)
        {
            data=other.data;
            other.data =nullptr;
        }

        //copy assignment

        Base& operator = (const Base& other)
        {
            if(this == &other)
                return *this;
            
            delete data;

            data = new int;
            *data =*(other.data);
            return *this;
        }

        Base& operator= (Base&& other) noexcept
        {
            if(this == &other)
                return *this;
            
            delete data;

            data=other.data;
            other.data=nullptr;
            return *this;
        }

        ~Base()
        {   
            cout<<"destructor called"<<endl;
            delete data;
        }
};

int main() {
    Base* b =new Base(5);
    delete b;
}