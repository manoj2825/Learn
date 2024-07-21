#include <iostream>
#include <vector>
#include <set>
#include    <unordered_set>
#include    <unordered_map>
#include <map>
using namespace std;

void solve(vector<int>& nums)
{
    unordered_set<int> s;
    
    for(int num: nums)
    {
        if(s.count(num) ==1)
        {
            cout<<num<< " ";
        }
        else{
            s.insert(num);
        }
    }
}

void solvemap(vector<int> &nums)
{
    map<int,int> m;

    for(int num:nums)
    {
        m[num]++;
    }

    for(auto &pair : m)
    {
        if(pair.second>1)
        {
            cout<<pair.first<< " ";
        }
    }
}

int main() {

  vector<int> arr = {1,2,2,3,3};

    solve(arr);
    cout<<endl;
    solvemap(arr);

}