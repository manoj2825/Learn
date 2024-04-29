#include <iostream>
using namespace std;
int add(int a,int b)
{
    return a+b;
}
int* createarray(int size)
{

    int * arr =new int[size];
    return arr;
    
}
void makesquare(int* x)
{
    int ans=(*x)*(*x);//deference the pointer to get the value
}

int main() {
    
    int x=5;
    //passing pointer to function
    makesquare(&x);


    //returning pointers from functions
    //when fun needs to allocate memory dynalically

    int* ptrs =createarray(5);

    delete[] ptrs;


    ///fucntion to the pointer
    //Pointers to functions are used in various scenarios like callback functions, 

  // Declare a pointer to a function that takes two integers and returns an integer

  int (*ptr)(int,int);
    ptr=&add; // Assign the address of the add function to ptr

    int result =ptr(5,6);
    //can implemet in callback function

    
}