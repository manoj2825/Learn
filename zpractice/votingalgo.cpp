#include <iostream>
#include <vector>
using namespace std;

int findMajorityElement(const std::vector<int>& nums) {

    int candidate =-1;
    int count =0;

    for(int num:nums)       //{1,2,1,1,1,1,2,2,2,1};
    {
        if(count==0)
        {
            candidate=num;
        }
        else if(num == candidate)
        {
            count++;
        }
        else{
            count--;
        }
    }

    count=0;

    for(int num:nums)
    {
        if(num == candidate)
        {
            count++;
        }
    }

    int midsize = nums.size()/2;

    if(count>midsize) return candidate;

    return -1; // No majority element found
}

int main() {
    std::vector<int> nums = {1,2,1,1,1,1,2,2,2,1};
    int result = findMajorityElement(nums);
    
    if (result != -1) {
        std::cout << "The majority element is: " << result << std::endl;
    } else {
        std::cout << "No majority element found." << std::endl;
    }

    return 0;
}