#include <iostream>
#include <climits>
#include <vector>
#include <algorithm>

using namespace std;

int longestConsecutive(vector<int> &nums)
{

    // int min = -1;

    // for (int num : nums)
    // {
    //     if (num < min)
    //     {
    //         min = num;
    //     }
    // }

    sort(nums.begin(),nums.end());
    int result = 0;

    for(int i = 0,j=i+1;j<nums.size();)
    {
       while(nums[i++] != nums[j++]+2)
       {
        result++;
       }
    }

    return result;
}

int main()
{
    vector<int> nums = {100,4,200,1,3,2};

    cout<<longestConsecutive(nums)<<endl;

}