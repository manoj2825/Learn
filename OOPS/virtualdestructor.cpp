#include <iostream>
#include "..\Basics\base\manoj.h"
using namespace std;

class base{
    public:
        base()
        {
            cout<<"base constructed"<<endl;
        }
        virtual ~base()
        {
            cout<<"base destructed"<<endl;
        }
};

class derived :public base{

    public:
        derived()
        {
            cout<<"derived constructed"<<endl;
        }
        ~derived()
        {
            cout<<"derived destructed"<<endl;
        }
};

int main() {
    
    base* b =new derived();

    //base b;
    delete b;

    return 0;
}