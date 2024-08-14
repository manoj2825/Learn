#include <iostream>
#include <climits>
#include <vector>
#include <algorithm>

using namespace std;

void printvector(vector<int>& nums)
{
    for(int num:nums)
    {
        cout<<num<<" ";
    }
}

bool fun(int a, int b)
{
    return a < b;
}

int main()
{
    vector<int> nums={6,2,3,4};

    sort(nums.begin(),nums.end(),fun);
    printvector(nums);

}