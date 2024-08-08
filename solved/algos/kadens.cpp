#include <iostream>
#include <vector>
#include <algorithm> // For std::max

int maxSubArray(const std::vector<int>& nums) {
    // Initialize with the first element
    int max_current = nums[0];
    int max_global = nums[0];

    // Iterate through the array starting from the second element
    for (size_t i = 1; i < nums.size(); i++) {
        // Update max_current: choose between the current element alone
        // or extending the current subarray with the current element
        max_current = std::max(nums[i], max_current + nums[i]);

        // Update max_global if the current subarray has a larger sum
        if (max_current > max_global) {
            max_global = max_current;
        }
    }

    return max_global;
}

int main() {
    std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    int max_sum = maxSubArray(nums);
    std::cout << "The maximum subarray sum is: " << max_sum << std::endl;
    return 0;
}
