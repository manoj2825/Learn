#include <iostream>
#include <vector>
using namespace std;

int closesToZero(vector<int>& nums)
{   
    int closest = nums[0];
    for(auto num:nums)
    {
        if(abs(num)<abs(closest))
        {
            closest = num;
        }
        else if(abs(num) == abs(closest) && num > closest) //for -1 and 1 condition
        {
            closest  = num;
        }
    }
    return closest;
}

int main() {
    
    vector<int> nums  = {-4,-2,-1,1,4,8};
    cout<<closesToZero(nums)<<endl;
}