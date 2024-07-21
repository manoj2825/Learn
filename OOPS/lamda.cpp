#include<iostream>

using namespace std;

//[]()->{} syntax
//[]capture clause
//()parameter
//-> return type
//{}body

auto add=[](int a,int b)-> int
{
    return a+b;
};



int main()
{
    int ans =add(5,3);//using lamda function
    cout<<ans<<endl;
}