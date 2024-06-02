#include <iostream>

using namespace std;

int main() {
    int var=5;
    cout<<var++<<endl;
    cout<<++var<<endl;
    
    

    int arr[5]={10,20,30,40,50};

    int top =4;

    cout<<arr[top]<<endl;
    arr[top--];
      cout<<arr[top]<<endl;
      cout<<arr[4]<<endl;
}