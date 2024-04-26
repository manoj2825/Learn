#include <iostream>
#include "manoj.h"
using namespace std;
void solve(vector<int>& nums, int target)
{   int n =nums.size();
int sum =0;
    for(int i =0;i<n;i++)
    {
        for(int j=1;j<n;j++)
        { 
            if(nums[j]==target -nums[i])
            {
                cout<<"yes"<<endl;
                cout<< "index are : "<< i<<" "<<j<<endl;
                return;
            }
           
        }
    }
    cout<<"no"<<endl;
}

int main() {
    vector<int> nums ={2,1,5,3};
    
    int target =8;
    solve(nums ,target);
}