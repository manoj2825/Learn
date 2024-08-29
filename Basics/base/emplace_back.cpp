#include <iostream>
#include <climits>
#include <vector>

using namespace std;

class Base
{
    public:
        int data;
        Base(int val)
        {
            data = val;
            cout<<"created "<<endl;
        }
};

int main()
{
    vector<int> v;
    vector<Base> vec;

    //push back will add the copy to the vector 
    //but fot int,char,float or primitive type there is no difference 
    //meaning both will act as same

    int x = 10;
    v.push_back(x);// or v.push_back(10);

    //but for emplace it direcly constructs

    v.emplace_back(20);

   // Using push_back (constructs the Base first, then copies into vector)
    Base b(30);
    vec.push_back(b);  // Copy constructor is called here

    // Using emplace_back (directly constructs the Base inside the vector)
    vec.emplace_back(25);  // Directly constructs Base in place, no copy
}