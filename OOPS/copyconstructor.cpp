#include <iostream>
#include "..\Basics\base\manoj.h"

using namespace std;

class base
{
private:
    int val;
    string name;
    int* ptr;
public:
    base();
    base(int x, string y,int ptrval)//shallowcopy
    {
        val=x;
        name=y;
        ptr= new int(ptrval);
        cout<<"shallow"<<endl;
    }

    base(base& other):val(other.val),name(other.name)
    {
        ptr=new int;
        *ptr = *(other.ptr);
        cout<<"deep"<<endl;
    }
    ~base();
};
base::base()
{
    cout<<"normal"<<endl;
}
base::~base()
{
    cout<<"destroyed"<<endl;
}


int main() {
    
    base* obj =new base(5,"manoj",5);

    base* obj1=new base(*obj);


    delete obj;
    delete obj1;
}