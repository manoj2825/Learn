#include<iostream>

using namespace std;

//factorial 3-> 3x2x1
//fact[3]=fact[2]

int fact(int num)
{
    if(num==1)
    {
        return 1;
    }
    return num * fact(num-1);
}

void factorial(int number){
    int sum=1;
    if(number ==1)
    {
        cout<<number<<endl;
        return;
    }
    for(int i=number ;i>0;i--)
    {
        sum=sum*i;
    }
    cout<<"without recursion"<<endl;
    cout<<sum<<endl;
}


int main()
{

int ans=0;
ans=fact(5);
cout<<ans<<endl;
factorial(5);
}