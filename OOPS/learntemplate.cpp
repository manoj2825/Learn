#include <iostream>
#include "..\Basics\base\manoj.h"

using namespace std;

template<typename T>

void swapvalues(T& a,T& b)
{
    T temp =a;
    a=b;
    b=temp;
}

template<typename T>
T templearn(T& a , T& b)
{
  return a+b;
}

template<typename T, typename B>

 void swapwithtwotemplate(T& a, T& b)
 {
    T temp =a;
    a=static_cast<T>(b);
    b=static_cast<B>(temp);
 }

template <class T> void bubbleSort(T a[], int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = n - 1; i < j; j--)
            if (a[j] < a[j - 1])
                swap(a[j], a[j - 1]);
}

int main() {
    
    int x = 10, y = 20;
    cout << "Before swapping: x = " << x << ", y = " << y << endl;
    swapvalues(x,y);
    cout << "After swapping: x = " << x << ", y = " << y << endl;

     int a[5] = { 10, 50, 30, 40, 20 };
    int n = sizeof(a) / sizeof(a[0]);

    // calls template function
    bubbleSort<int>(a, n);

}