#include <iostream>
#include <climits>
#include <vector>

using namespace std;

class Test
{
    public:
        int* data;

        Test(int val):data(new int(val))
        {
            cout<<"constructed"<<endl;
        }

        ~Test();

        Test(const Test& other);

        Test& operator =(const Test& other);

        Test(Test&& other);

        Test& operator =(Test&& other);
};

Test::~Test()
{
    delete data;
    cout<<"destructed"<<endl;
}

Test::Test(const Test& other)
{
    data = new int;
    *data = *(other.data);
    cout<<"copy constructed"<<endl;
}

Test& Test:: operator=(const Test& other)
{
    if(this ==&other)
    {
        return *this;
    }

    delete data;

    data = new int;
    *data = *(other.data);
    cout<<"copu assignment constructed"<<endl;
    return *this;
}

Test::Test(Test&& other)
{
    data = other.data;
    other.data = nullptr;
    cout<<"move constructed"<<endl;
}

Test& Test:: operator=(Test&& other)
{
    if(this ==&other)
    {
        return *this;
    }

    delete data;

    data = other.data;
    other.data = nullptr;
    cout<<"move assignment constructed"<<endl;
    return *this;
}
int main()
{
    // Test* t = new Test(5);
    // Test* t1  = new Test(*t);
    // delete t;
    Test t(5);

    Test t1(t);

    Test t2(2);

    t2 = t1;

    Test t4 = move(t1);

    t2 = move(t);

}