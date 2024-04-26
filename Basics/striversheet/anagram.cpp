#include <iostream>
#include "manoj.h"
typedef bool BOOL;
using namespace std;

BOOL solve(string s, string t)
{
    if(s.length()!=t.length())
    {
        return false;
    }

    unordered_map<char,int> charCount;

    for(char i: s)
    {
        charCount[i]++;
    }

    for (char c : t) {
        if (charCount.find(c) == charCount.end() || charCount[c] == 0) {
            return false; // Character not found in s or count becomes negative
        }
        charCount[c]--;
    }
    return true;
}


int main() {
    string s="rat";
    string t ="car";

    if(solve(s,t))
    {
        cout<<"yes"<<endl;
    }
    else{
        cout<<"No"<<endl;
    }
}