#include <iostream>
#include <climits>
#include <vector>

using namespace std;

void fun(int a, int b=0,int c=0);//when b is 0, you need to provide the default paramter for rest paramets

namespace Globalbank
{
    struct Bankaccount
    {

    };

    void createaccount(const Bankaccount& b)
    {
        cout<<"bank balance"<<endl;
    }
}

    struct Bankaccount
    {

    };

    void createaccount(const Bankaccount& b)
    {
        cout<<"No balance"<<endl;
    }

Globalbank::Bankaccount account;


void fun(int a, int b,int c)
{
    cout<<"called with b : "<<b<<endl;
}

int main()
{
    unsigned char a = 25;
    
    a=~a;
    cout<<(int)a<<endl;//230

    signed char b = 25;
    
    b=~b;
    cout<<(int)b<<endl;//-26

    createaccount(account);

    int x =10;
    //fun(5,5);
    fun(5,6,7);
    //fun(x,5);

}