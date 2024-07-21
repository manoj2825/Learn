#include <iostream>
#include <string.h>
#include <algorithm>
using namespace std;

void reversestring(string &str)
{
    int n =str.length();
    for(int i=0;i<n/2;i++)
    {
        swap(str[i],str[n-1-i]);
        //n--;
    }
}

int main() {

    string name = "manoj";
    reversestring(name);
    cout<<name<<endl;
}