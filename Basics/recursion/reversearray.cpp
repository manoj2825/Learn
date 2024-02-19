#include <iostream>
using namespace std;

void display(int arr[],int size)
{
    for(int i=0;i<size;++i)
    {
        cout<<arr[i]<<"\t";
    }
}

void reverse(int arr[],int size)
{   int mid=size/2;
int end= size-1;
    for(int i=0;i<mid;++i)
    {
        int temp=arr[i];
        arr[i]=arr[end];
        arr[end]=temp;
        end--;
    }
}


int main()

{
    int arr[5]={1,2,3,4,5};
    int size=sizeof(arr)/sizeof(arr[0]);
    reverse(arr,size);
    display(arr,size);

}