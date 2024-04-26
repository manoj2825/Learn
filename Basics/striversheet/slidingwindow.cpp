#include <iostream>
#include "manoj.h"
using namespace std;

int naive(int arr[], int n, int k)
{
    // Initialize result
    int max_sum = 0;
    // approrach 1
    //  Consider all blocks starting with i.
    //  for (int i = 0; i < n - k + 1; i++) {
    //      int current_sum = 0;
    //      for (int j = 0; j < k; j++)
    //          current_sum = current_sum + arr[i + j];

    //     // Update result if required.
    //     max_sum = max(current_sum, max_sum);
    // }

    // approach two
    for (int i = 0; i <= n - k; i++)
    {
        int current_sum = 0;
        for (int j = i; j < i + k; j++)
        {
            current_sum = current_sum + arr[j];
        }
        max_sum = max(current_sum, max_sum);
    }

    return max_sum;
}

int optimal(int arr[], int n, int k)
{
    if (n <k)
    {
        cout<<"Imvalid"<<endl;
        return -1;
    }

    //calculate the first window sum
    int max_sum=0;
    for(int i=0;i<k;i++)
    {
        max_sum+=arr[i];
    }

    //now assign this valus to windowsum;
    int windowsum=max_sum;

    //iterate through array and do the slide

    for(int i=k ;i<n;i++)
    {
        windowsum=windowsum + arr[i]-arr[i-k];//add the current index and minus the used index
        //max_sum=max(max_sum,windowsum);
        max_sum = max(max_sum, windowsum);

    }
    return max_sum;
}
int main()
{
    int arr[] = {1, 4, 2, 10, 2, 3, 1, 0, 20};
    int k = 4;
    int n = sizeof(arr) / sizeof(arr[0]);
    //cout << naive(arr, n, k);

    cout << optimal(arr, n, k);
}