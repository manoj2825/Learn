#include <iostream>
#include <climits>
#include <vector>
#include <unordered_map>
using namespace std;

bool isSubsequence_withoutorder(string s, string t) {
        
    unordered_map<char,int> mp;

    for(auto ch:s)
    {
        mp[ch]++;
    }

    for (char ch : t) {
        if (mp[ch] > 0) {
            mp[ch]--; // Use up one occurrence
        } else {
            return false; // If character in t is not present in s, return false
        }
    }
    return true;
}

bool isSubsequence(string s, string t) {
        
    int s_len = s.length();
    int t_len = t.length();
    int s_index = 0;
    int t_index = 0;
    
    while(s_index < s_len  &&  t_index<t_len)
    {
        if(s[s_index] == t[t_index])
        {
            s_index++;
        }
        t_index++;
    }

    return s_index == s_len;
}

int main()
{
    string t = "abcde";
    string s = "aec";

    cout<<(isSubsequence(s,t)? "Valid":"Not valid")<<endl;
}