#include <iostream>
#include "manoj.h"
using namespace std;

void solve(vector<int> &arr, int n)
{
    int temp = arr[0];
    for (int i = 0; i < n; i++)
    {
        arr[i] = arr[i + 1];
    }

    arr[n - 1] = temp;
}


int main()
{

    vector<int> arr = {1, 2, 3, 4, 5};
    int n = arr.size();

    solve(arr, n);

    for(auto it:arr)
    {
        cout<<it<< ' ';
    }
}