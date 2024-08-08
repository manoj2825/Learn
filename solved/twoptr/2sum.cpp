#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

pair<int,int> solve (vector<int> nums,int target)
{
    for(int i=0;i<nums.size();i++)
    {
        for(int j=1;j<nums.size();j++)
        {
            if(nums[i]+nums[j] == target)
            {
                return {i,j};
            }
        }
    }
    return {-1,-1};
}

pair<int,int> withtwopointer (vector<int> nums,int target)
{
    int left =0;
    int right =nums.size()-1;

    while(left<right)
    {
        int sum =nums[left]+nums[right];

        if(sum==target)
        {
            return {left,right};
        }else if(sum<target){
            left++;
        }else{
            --right;
        }
    }
    return {-1,-1};
}

int main() {

    //given sorted array
    vector<int> nums = {2, 3, 4, 5, 6, 7};
    int target = 9;
    
   // auto ans =solve(nums,target);
   auto ans =withtwopointer(nums,target);

    cout<<ans.first << " "<<"element : "<<nums[ans.first]<<endl;
    cout<<ans.second<< " "<<"element : "<<nums[ans.second]<<endl;
    
}