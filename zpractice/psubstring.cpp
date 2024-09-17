#include <iostream>
#include <climits>
#include <vector>
#include <unordered_map>

using namespace std;

class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        
        unordered_map<char,int> charIndex;

        int start = 0;
        int maxLength = 0;

        for(int end = 0;end<s.length();++end)
        {
            char currentchar = s[end];

            //check if the char is alreafy in map
            //if yes then update the start +1 fromt thr current char index

            if(charIndex.find(currentchar) != charIndex.end())
            {
                start = max(start,charIndex[currentchar]+1);
            }

            charIndex[currentchar] = end;

            maxLength = max(maxLength,end-start+1);
        }
        return maxLength;
    }
};

int main()
{
    string str = "mbcbab";
    Solution s;
    cout<<s.lengthOfLongestSubstring(str)<<"\n";   
}