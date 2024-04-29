#include <iostream>
#include "manoj.h"

using namespace std;



void callbackfunction(int data)
{
    cout<<"this is callbackfunction and the data is : "<<data<<endl;
}

void testfunction(int data, void (*callback)(int))//here you add the finction pointer as argument
{
    cout<<"this is testfunction "<<endl;

    //calling callbackfunction

    callback(data); //called using the pointer to the function
}

int main() {

    testfunction(10,callbackfunction);
    return 0;
}   