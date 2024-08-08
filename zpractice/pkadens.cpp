#include <iostream>
#include <climits>
#include <vector>
#include<algorithm>

using namespace std;

int maxSubArray(vector<int>& nums)
{
    int msf =nums[0];//msf -> max so far
    int currentmax =nums[0];

    for(int i=1;i<nums.size();++i)
    {
        currentmax = max(nums[i],currentmax+nums[i]);
        msf = max(msf,currentmax);
    }
    return msf;
}

int main()
{
    std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    int max_sum = maxSubArray(nums);
    std::cout << "The maximum subarray sum is: " << max_sum << std::endl;
    return 0;
}