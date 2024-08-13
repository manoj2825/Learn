#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;


vector<int> sortwithfrequency(vector<int>& nums)
{
    unordered_map<int,int> mp;

    for(auto num:nums)
    {
        mp[num]++;
    }

    sort(nums.begin(),nums.end(),[&](int a, int b) ->bool
    {   
        if (mp[a] != mp[b]) {
            return mp[a] < mp[b];
        }
        return a > b;

    });

    return nums;
}
void print(vector<int>& nums)
{
    for(auto num:nums)
    {
        cout<<num<<" ";
    }
    cout<<endl;
}

//sort in descending order
bool fun(int a, int b)
{
    return a > b;
}

int main() {
    
    //vector<int> nums = {2,1,6,3,6,4};//here b=2 and a=1
    vector<int> nums = {2,3,1,3,2};//here b=2 and a=1

    //sort(nums.begin(),nums.end(),fun);//normal desceding sort
    print(nums);

    vector<int> ans = sortwithfrequency(nums);
    print(ans);
}