#include <iostream>
#include <vector>
using namespace std;


int maxSumSubarray(vector<int>& arr, int k)
{
    int n =arr.size();
    if(k>n) return -1;

    //calculate the first window sum
    int max_sum=0;
    for(int i=0;i<k;i++)
    {
        max_sum+=arr[i];
    }

    int window_sum=max_sum;

    //now apply the logic
    for(int i=k;i<n;i++)
    {
        window_sum +=arr[i]-arr[i-k];
        max_sum=max(window_sum,max_sum);
    }
    return max_sum;

}

int main() {

    vector<int> arr = {2, 1, 5, 1, 3, 2};
    int k = 3;
    cout << "Maximum sum of subarray of size " << k << " is " << maxSumSubarray(arr, k) << endl;
}