#include <iostream>
using namespace std;

class Base
{
    public:
        //static int count = 0;//this is wrong because you cannot init inside the class for static variable
        static int count;
        Base(){}

        static void  countmethod()
        {
            count++;
        }
};

int Base::count = 0;

int main() {
    
    cout << "Initial count: " << Base::count << endl;

    Base::countmethod();
    cout << "Count after calling countmethod: " << Base::count << endl;

    Base::countmethod();
    cout << "Count after calling countmethod again: " << Base::count << endl;
}