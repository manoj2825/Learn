#include <iostream>
#include "..\Basics\base\manoj.h"

using namespace std;

class test{
    private:
        ~test(){}
    
    public:
        friend void deletefunc(test*);
};


void deletefunc(test* t)
{
    delete t;
}
int main() {
    
    //test t; //throws the error saying private
    //if you create dynamically its programmers job to delete
    //so no error 
    //test* t;// no error
    //if you try to delete this 
    //delete t//you will get the error

    // so lets use the friend function approach
    test* t =new test();

    deletefunc(t);


    return 0;
}