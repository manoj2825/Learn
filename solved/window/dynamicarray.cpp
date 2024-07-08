#include <iostream>
#include <vector>
#include <climits>
using namespace std;

int smallestSubarrayWithSum(vector<int>& arr, int S) {
    int n = arr.size();
    int min_length = INT_MAX, window_sum = 0, left = 0;

    for (int right = 0; right < n; right++) {
        window_sum += arr[right];

        while (window_sum >= S) {
            min_length = min(min_length, right - left + 1);
            window_sum -= arr[left++];
        }
    }

    return min_length == INT_MAX ? 0 : min_length;
}

int main() {
    vector<int> arr = {2, 1, 5, 2, 3, 2};
    int S = 7;
    cout << "Length of the smallest subarray with sum " << S << " is " << smallestSubarrayWithSum(arr, S) << endl;
    return 0;
}