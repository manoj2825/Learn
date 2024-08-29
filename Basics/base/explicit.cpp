#include <iostream>
#include <climits>
#include <vector>

using namespace std;

class Base
{
    public:
        explicit Base(int x)
        {
            cout<<"constructor called withe the value : "<<x<<endl;
        }

};

int main()
{
    //this is the example for implicit conversion
    //Base b =5;//you can create like this untill and unless explicit is used

    //but if you use the keyword
    Base b(5);
}