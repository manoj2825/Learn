#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <climits>
using namespace std;

bool binarysearch(vector<int> &nums, int key)
{
    int low =0;
    int high = nums.size()-1;
    //int mid = (low+high)/2;
    while(low<=high)
    {
         int mid = low + (high - low) / 2;
        if(nums[mid] == key) return true;
        if(nums[mid]<key)
        {
            low =mid+1;
        }
        if(nums[mid]>key)
        {
            high =mid-1;
        }
    }
    return false;
}

//max sum of 3 in array
int fixedslide(vector<int> &nums,int k)
{
    //first calculate the window sum
    int totalsum =0;
    for(int i=0;i<k;i++)
    {
        totalsum+=nums[i];
    }
    int windowsum = totalsum;

    for(int i=k;i<=nums.size();i++)
    {
        windowsum =windowsum +nums[i]-nums[i-k];
        totalsum = max(windowsum,totalsum);
    }
    return totalsum;
}

int minlength(vector<int> &nums,int sum)
{
    int n =nums.size();
    int minLength=INT_MAX;
    int windowsum=0;
    int left = 0;
    for(int right =0;right<n;right++)
    {
        windowsum+=nums[right];
        while(windowsum>=sum)
        {
            minLength = min(minLength,right-left+1);
            windowsum-=nums[left++];
        }
    }
    return minLength==INT_MAX?0:minLength;

}

int maxlength(vector<int> & nums, int sum)
{
    int n=nums.size();
    int left=0;
    int windowsum=0;
    int maxLength  = 0;

    for(int right = 0;right<n;right++)
    {
        windowsum+=nums[right];

        while(windowsum>sum)
        {
            windowsum-=nums[left++];
        }
        maxLength = max(maxLength,right-left+1);
    }
    return maxLength==0?-1:maxLength;
}

int main() {

    vector<int> nums = {1,3,6,1,0,1,8,10};
    // sort(nums.begin(),nums.end());
    // if(binarysearch(nums,10))
    //     cout<<"found"<<endl;
    // else
    //     cout<<"Not found"<<endl;

    //cout<<fixedslide(nums,3)<<endl;

     cout<<minlength(nums,10)<<endl;
     cout<<maxlength(nums,10)<<endl;

}