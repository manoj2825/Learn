#include <iostream>
#include <climits>
#include <vector>

using namespace std;

void printvec(vector<int>& nums)
{
    if(nums.empty()) cout<<"its empty"<<endl;
    for(int num:nums)
    {
        cout<<num<< " ";
    }
    cout<<endl;
}

int main()
{
    vector<int> nums = {2,3,7,8,5,1,3,5,1,9};

    printvec(nums);

    cout<<"size : "<<nums.size()<<endl;
    cout<<"max - size : "<<nums.max_size()<<endl;
    cout<<"capacity : "<<nums.capacity()<<endl;

    cout<<"operator : "<<nums.operator[](5)<<endl;
    cout<<"at : "<<nums.at(9)<<endl;
    cout<<"front : "<<nums.front()<<endl;
    cout<<"back : "<<nums.back()<<endl;
    cout<<"data : "<<nums.data()<<endl;

    vector<int>::iterator it;
    for(it =nums.begin();it!=nums.end();++it)
    {
        cout<<*it<<" ";
    }
    cout<<"\n";

    //simple program to return the indedx of the target using iterator
    int index = 0;
    for(it =nums.begin();it!=nums.end();++it)
    {
        if(*it == 7)
        {
            cout<<"index of target is : "<<index<<endl;
        }
        ++index;
    }

    nums.clear();
    printvec(nums);
}