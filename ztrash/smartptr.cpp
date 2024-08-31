#include <iostream>
#include <climits>
#include <vector>
#include <memory>

using namespace std;



int main()
{
    unique_ptr<int> ptr1= make_unique<int>(10);

    cout<<*ptr1<<endl;

    unique_ptr<int> ptr2 = move(ptr1);//cannot be copied like ptr2 =ptr1

    cout<<*ptr2<<endl;


    shared_ptr<int> sptr1 = make_shared<int>(100);
    cout<<*sptr1<<endl;

    //can be copied
    shared_ptr<int> sptr2 = sptr1;

    cout<<*sptr2<<endl;
    
    cout<<sptr2.use_count()<<endl;//to find how many references are being used


    weak_ptr<int> weak;
    weak = sptr1;

    if(auto sptr2 = weak.lock())
    {
        cout<<*sptr2<<endl;
    }
    //ptr1.reset(); // Reference count is 0, Resource is destroyed
}