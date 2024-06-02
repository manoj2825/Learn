//using array
#include <iostream>
//#include "..\Basics\base\manoj.h"

using namespace std;

class Stack
{
    private:
        int* arr;
        int top;
        int capacity;

    public: 
        Stack(int size);
        void push(int x);
        int pop();
        bool isFull();
        bool isEmpty();
        int peek();
        int size();
};

Stack::Stack(int size)
{
    arr = new int[size];
    top=-1;
    capacity =size;
}

void Stack::push(int x)
{
    if(isFull())
    {   
        cout<<"Stack Overloaded"<<endl;
        exit(EXIT_FAILURE);
    }

    cout<<"Pushing ... : "<<x<<endl;
    arr[++top]=x;
}

int Stack::pop()
{
    if(isEmpty())
    {   
        cout<<"Stack isEmpty"<<endl;
        exit(EXIT_FAILURE);
    }

    cout<<"Popping ... : "<<peek()<<endl;
    return arr[top--];
}

bool Stack::isFull()
{
    return top==capacity-1;
}

bool Stack::isEmpty()
{
    return top==-1;
}

int Stack::peek()
{   if(!isEmpty())
    {
    return arr[top];
    }
    else{
        exit(EXIT_FAILURE);
    }
}

int Stack::size() {
    return top + 1;
}


int main() {
    
   Stack s(5);

   for(int i=0;i<5;i++)
   {
    s.push(i);
   }
    cout<<s.peek()<<endl;
   cout<<s.pop()<<endl;
}