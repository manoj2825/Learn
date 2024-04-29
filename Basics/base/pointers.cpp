#include <iostream>
using namespace std;

int main() {
    
    //decalre a ponter type int

    int* ptr;//pointer to random addresss

    int val=5;
    ptr =&val; //holds the address of val
    cout<<*ptr<<endl;//you can print the value
    cout<<ptr<<endl;//prints the address

    int* test =nullptr; //can decalre the null ptr points to nowhere

    //dynamic allocation

    int* testptr = new int; //assigns garbage value
    *testptr = 10;
    cout<<*testptr<<endl;

    //array pointers

    int arr[5]={1,2,3,4,5};
    int* arrptr =arr;//points to afirst element

    cout<<*arrptr<<endl;//prints first element
    arrptr++;//can move to enext element
    cout<<*arrptr<<endl;

    //double pointer
    int value =5;
    int* ptrone =&value;
    int** secondptr=&ptrone;//pointer to the pointer
    int*** thirdptr =&secondptr;


    
}