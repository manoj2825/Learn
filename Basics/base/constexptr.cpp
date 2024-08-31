#include <iostream>
#include <climits>
#include <vector>

using namespace std;

constexpr int sqaure(int x)
{
    return x*x;
}

int main()
{   
     int ans = sqaure(5);//evaluated at run time becaue ans is not constexpr

    constexpr int result  = sqaure(10); //evaluted at comipler

    //just to make sure that this is evaluted at compile tine 
    //int arr[ans];// will throw erroer because ans is yet to be evalued in run time 
    int arr[result];//wont throw anything 

}