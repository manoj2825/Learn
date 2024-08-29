#include <iostream>
#include <climits>
#include <vector>

using namespace std;

class Test
{
    private:
        int data;

    public:
        Test():data(0){}
        friend void setdata(Test& t,int x);


};

void setdata(Test& t,int x)
{
    t.data = x;
}

int main()
{
    int a =1;
    cout<<(a++)*(++a)<<endl;
}