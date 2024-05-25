#include <iostream>
#include "..\Basics\base\manoj.h"

using namespace std;

template<typename T>
class Test{
    private:
        T data;
    public:

        void setdata(T value);

        T getdata();

};

template<typename T>
void Test<T> :: setdata(T value)
{
    data =value;
}

template<typename T>
T Test<T>:: getdata()
{
    return data;
}

int main() {
    
    Test<int>* obj = new Test<int>();
    obj->setdata(5);
    cout<<obj->getdata();
    return 0;
}