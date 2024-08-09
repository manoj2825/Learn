#include <iostream>
#include <climits>
#include <vector>

using namespace std;



int main()
{
    char name[] = "manojsunagar";
    string names = "manojsunagar";
    cout<<&name[5]<<endl;
    cout<<&names[5]<<endl;

    int arr[]={1,2,4,4,5,5};
    cout<<&arr[3]<<endl;
}