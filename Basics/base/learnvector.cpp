#include <iostream>
#include <vector>
using namespace std;

int main() {
    
    vector<int> nums;

    for(int i =1;i<=5;i++)
    {
        nums.push_back(i);
    }

    for(auto i=nums.begin();i!=nums.end();i++)
    {
        cout<<(*i)<<"\t";
    }
    cout<<endl;
     for(auto i=nums.rbegin();i!=nums.rend();i++)
    {
        cout<<(*i)<<"\t";
    }
        cout<<endl;
    for(auto i=nums.crbegin();i!=nums.crend();i++)
    {
        cout<<(*i)<<"\t";
    }
}
