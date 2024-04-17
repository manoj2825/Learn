#include <iostream>
#include <vector>
#include <climits>
using namespace std;

vector<int> solve(vector<int> &nums, int n)
{
    int small = INT_MAX;
    int second_small = INT_MAX;
    int large = INT_MIN;
    int second_large=INT_MIN;

    if(n<2)
    {
        return {-1,-1};
    }
    for(int i=0;i<n;i++)
    {
        int temp = nums[i];
        if(nums[i]<small)
        {
            second_small = small;
            small=nums[i];
        }
        else if (nums[i]<second_small)
        {
           second_small =nums[i];
        }

        if(nums[i]>large)
        {
            second_large = large;
            large=nums[i];
        }
        else if (nums[i]>second_large)
        {
           second_large =nums[i];
        }
        
    }
    return {second_large,second_small};
}

int main() {
    vector<int> nums = {1,2,3,4,5};
    int n=nums.size();

    vector<int> ans = solve(nums,n);
    //cout << "Second Largest: " << ans[0] << ", Second Smallest: " << ans[1] << endl;
    for(auto i=ans.begin();i!=ans.end();i++)
    {
        cout<<*i<<"\t";
    }
}