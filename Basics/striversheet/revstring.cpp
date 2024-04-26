#include <iostream>
#include "manoj.h"
using namespace std;
void usingnormal(string &str)
{
    int s = str.length();
    for (int i = 0; i < s / 2; i++)
    {
        swap(str[i], str[s - i - 1]);
    }
}

void usingwhile(string &str)
{
    int s = str.length()-1;
    int i =0;
    while(s>i)
    {
        swap(str[i], str[s]);
        i++;
        s--;
    }
}
int main()
{
    string str = "manoj";
    //usingnormal(str);
    usingwhile(str);
    cout << str << endl;
}