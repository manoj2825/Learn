//using array
#include <iostream>
//#include "..\Basics\base\manoj.h"

using namespace std;

template<typename T>
class Stack
{
    private:
        T* arr;
        int top;
        int capacity;

    public: 
        Stack(int size);
        void push(int x);
        void pop();
        bool isFull();
        bool isEmpty();
        int peek();
        int size();
};

template<typename T>
Stack<T>::Stack(int size)
{
    arr = new int[size];
    top=-1;
    capacity =size;
}

template<typename T>
void Stack<T>::push(int x)
{
    if(isFull())
    {   
        cout<<"Stack Overloaded"<<endl;
        exit(EXIT_FAILURE);
    }

    cout<<"Pushing ... : "<<x<<endl;
    arr[++top]=x;
}

template<typename T>
void Stack<T>::pop()
{
    if(isEmpty())
    {   
        cout<<"Stack isEmpty"<<endl;
        exit(EXIT_FAILURE);
    }

    cout<<"Popping ... : "<<peek()<<endl;
    arr[--top];
}

template<typename T>
bool Stack<T>::isFull()
{
    return top==capacity-1;
}

template<typename T>
bool Stack<T>::isEmpty()
{
    return top==-1;
}

template<typename T>
int Stack<T>::peek()
{   if(!isEmpty())
    {
    return arr[top];
    }
    else{
        exit(EXIT_FAILURE);
    }
}

template<typename T>
int Stack<T>::size() {
    return top + 1;
}


int main() {
    
   Stack<int>  s(5);

   for(int i=0;i<5;i++)
   {
    s.push(i);
   }
    cout<<s.peek()<<endl;
   s.pop();
   s.pop();
   s.pop();
}