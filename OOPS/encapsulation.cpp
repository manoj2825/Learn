#include <iostream>
#include "..\Basics\base\manoj.h"

using namespace std;

class base
{
private:
   int val;
public:
    void setdata(int x)
    {
        val=x;
    }

    int getdata()
    {
        return val;
    }



};



int main() {
    
    base obj;

    obj.setdata(3);

    cout<<obj.getdata();

}