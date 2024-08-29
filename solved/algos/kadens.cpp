#include <iostream>
#include <vector>
#include <algorithm> 
using namespace std;

int maxSubArray(const std::vector<int>& nums) {
    // Initialize with the first element
    int msf = nums[0];
    int maX = nums[0];

    // Iterate through the array starting from the second element
    for (size_t i = 1; i < nums.size(); i++) {
        // Update max_current: choose between the current element alone
        // or extending the current subarray with the current element
        msf = max(nums[i], msf + nums[i]);

        // Update max_global if the current subarray has a larger sum
        maX = max(maX,msf);
    }

    return maX;
}

int main() {
    std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    int max_sum = maxSubArray(nums);
    std::cout << "The maximum subarray sum is: " << max_sum << std::endl;
    return 0;
}
