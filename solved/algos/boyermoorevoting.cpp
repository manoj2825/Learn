#include <iostream>
#include <vector>

int findMajorityElement(const std::vector<int>& nums) {
    int candidate = -1;
    int count = 0;

    // Step 1: Find the candidate for the majority element
    for (int num : nums) {
        if (count == 0) {
            candidate = num;
            count = 1;
        } else if (num == candidate) {
            count++;
        } else {
            count--;
        }
    }

    // Step 2: Verify if the candidate is actually the majority element
    count = 0;
    for (int num : nums) {
        if (num == candidate) {
            count++;
        }
    }

    if (count > nums.size() / 2) {
        return candidate;
    }

    return -1; // No majority element found
}

int main() {
    std::vector<int> nums = {1, 1, 1, 1, 3, 4, 2};
    int result = findMajorityElement(nums);
    
    if (result != -1) {
        std::cout << "The majority element is: " << result << std::endl;
    } else {
        std::cout << "No majority element found." << std::endl;
    }

    return 0;
}