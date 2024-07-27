#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

int lengthOfLongestSubstringKDistinct(const string& s, int k) {
    unordered_map<char, int> charCount;
    int n = s.size();
    int maxLength = 0;
    int left = 0;

    for (int right = 0; right < n; ++right) {
        charCount[s[right]]++;
        
        // Shrink the window until we have at most k distinct characters
        while (charCount.size() > k) {
            charCount[s[left]]--;
            if (charCount[s[left]] == 0) {
                charCount.erase(s[left]);
            }
            left++;
        }
        
        maxLength = max(maxLength, right - left + 1);
    }

    return maxLength;
}

int main() {
    string s = "eceba";
    int k = 2;
    cout << "Length of the longest substring with at most " << k << " distinct characters: " << lengthOfLongestSubstringKDistinct(s, k) << endl;
    return 0;
}
