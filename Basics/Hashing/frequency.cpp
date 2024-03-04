#include <iostream>
using namespace std;



int main() {
    
     int n=5;
    // cin>>n;

    int arr[5];
    for(int i=0;i<n;i++)
    {
        cin>>arr[i];
    }

    //hashing method
    //assume hash for 12 elements

    int hash[12]={0};

    for(int i=0;i<n;i++)
    {
        hash[arr[i]]+=1;
    }

    int t;
    cin>>t;
    while(t--)
    {
        int number;
        cin>>number;

        cout<<hash[number]<<endl;

    }

}