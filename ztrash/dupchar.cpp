#include <iostream>
#include <climits>
#include <vector>
#include <unordered_set>

using namespace std;

void showdupchar(string& str)
{
    unordered_set<char> seen;

    for(char ch : str)
    {
        if(seen.count(ch)>0)
        {
            cout<<ch<<" ";
        }
        else
        {
            seen.insert(ch);
        }
    }
}

int main()
{
    string str = "abcbhajedb";
    showdupchar(str);
}