#include <iostream>
#include <vector>

using namespace std;

bool binary_search(vector<int>& nums,int target)
{
    int size =nums.size();
    int low=0;
    int high =size-1;
    int mid =(low+high)/2;
    while(low<=high)
    {
        if(nums[mid]==target) return true;
        if(nums[mid]>target)
        {
            low=mid+1;
        }
        if(nums[mid]<target)
        {
            high=mid-1;
        }
    }
    return false;
}

int main() {
    
    vector<int> a={1,2,3,4,5,6};
    int Target =4;
    if(binary_search(a,Target))
    {
         cout<<"target not found"<<endl;
    }
    else
    {
        cout<<"Found the target"<<endl;
    }
    
}