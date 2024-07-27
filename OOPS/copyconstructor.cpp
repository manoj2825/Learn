#include <iostream>
#include "..\Basics\base\manoj.h"

using namespace std;

class base
{
private:
    int val;#include <iostream>
#include <cstring>

class base {
private:
    int val;
    std::string name;
    int* ptr;

public:
    // Default constructor
    base();

    // Parameterized constructor (shallow copy)
    base(int x, std::string y, int ptrval) : val(x), name(y), ptr(new int(ptrval)) {
        std::cout << "shallow" << std::endl;
    }

    // Copy constructor (deep copy)
    base(const base& other) : val(other.val), name(other.name) {
        ptr = new int;
        *ptr = *(other.ptr);
        std::cout << "deep" << std::endl;
    }

    // Shallow copy constructor
    base(const base& other) : val(other.val), name(other.name), ptr(other.ptr) {
        std::cout << "shallow copy" << std::endl;
    }

    // Destructor
    ~base() {
        delete ptr;
        std::cout << "destroyed" << std::endl;
    }
};

base::base() {
    std::cout << "normal" << std::endl;
}

int main() {
    base* obj = new base(5, "manoj", 5);

    base* obj1 = new base(*obj);  // Deep copy constructor

    base* obj2 = new base(*obj1); // Shallow copy constructor

    delete obj;
    delete obj1;
    delete obj2;

    return 0;
}

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