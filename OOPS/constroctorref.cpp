#include <iostream>
using namespace std;

class Test
{
    public:
        int data;
        int temp = 5;

        Test(int value)
        {
            data =value;
        }
};

int main() {
    
    int test=9;
    Test* obj =new Test(2);
    cout<<obj->temp<<endl;
}