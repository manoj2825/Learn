#include <iostream>
using namespace std;

class Test
{
    public:
        int* data;

        Test(int val):data(new int(val)){}

        Test(const Test& other) = delete;
        Test& operator= (const Test& other) = delete;
        int* getdata()
        {
            return data;
        }
};

int main() {
    
    Test t(1);
    //cout<<*(t->getdata())<<endl;
    Test t2(4);
    //Test t3(t);
    //t2 = t;
}