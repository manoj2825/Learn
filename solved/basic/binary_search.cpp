#include <iostream>
#include <vector>

using namespace std;

bool binary_search(vector<int>& nums,int target)
{
    int size =nums.size();
    int low=0;
    int high =size-1;
    
    while(low<=high)
    {
        int mid =low+(high-low)/2;
        if(nums[mid]==target) return true;
        if(target < nums[mid])
        {
            high=mid-1;
        }
        else
        {
            low=mid+1;
        }
    }
    return false;
}

int selectinsert(vector<int>& nums,int target)
{
    int size =nums.size();
    int low=0;
    int high =size-1;
    
    while(low<=high)
    {
        int mid =low+(high-low)/2;
        if(nums[mid]==target) return mid;
        if(target < nums[mid])
        {
            high=mid-1;
        }
        else
        {
            low=mid+1;
        }
    }
    return low;
}

int main() {
    
    vector<int> a={1,3,5,6};
    int Target =7;
    if(!binary_search(a,Target))
    {
         cout<<"target not found"<<endl;
    }
    else
    {
        cout<<"Found the target"<<endl;
    }

    cout<<selectinsert(a,Target)<<endl;
    
}