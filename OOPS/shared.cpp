//how to create an instance of a class using shared pointer
//here i am using unique pointer

#include <iostream>
using namespace std;

class Base
{
    private:
        int val;
    
    public:
        Base(){}

        void setval(int _val)
        {
            this->val = _val;
        }

        int getval(){ return val;}

};

int main() {

    // Using unique_ptr
    unique_ptr<Base> uniquePtr = make_unique<Base>();
    uniquePtr->setval(10);
    cout << "unique_ptr: " << uniquePtr->getval() << endl;

    // Using shared_ptr
    shared_ptr<Base> sharedPtr = make_shared<Base>();
    sharedPtr->setval(20);
    cout << "shared_ptr: " << sharedPtr->getval() << endl;

    // Using weak_ptr
    weak_ptr<Base> weakPtr;
    {
        shared_ptr<Base> tempSharedPtr = make_shared<Base>();
        tempSharedPtr->setval(30);
        weakPtr = tempSharedPtr; // weak_ptr becomes valid
        cout << "Inside block - weak_ptr: " << tempSharedPtr->getval() << endl;
    } // tempSharedPtr goes out of scope here

    if (auto tempSharedPtr = weakPtr.lock()) { // Lock weak_ptr to check if it is still valid
        cout << "Outside block - weak_ptr: " << tempSharedPtr->getval() << endl;
    } else {
        cout << "Outside block - weak_ptr is expired" << endl;
    }
}