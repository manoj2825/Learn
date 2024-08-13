#include <iostream>
#include <climits>
#include <vector>

using namespace std;

void fun()
{
    static int x=10;
    x++;
    cout<<x<<endl;
}

int main()
{
    char name[] = "manojsunagar";
    string names = "manojsunagar";
    cout<<&name[5]<<endl;
    cout<<&names[5]<<endl;

    int arr[]={1,2,4,4,5,5};
    cout<<&arr[3]<<endl;

    int i =5;
    int j =++i;
    
    cout<<"i : "<<i<<endl;
    cout<<"j : "<<j<<endl;

    fun();//11
    fun();//12
}