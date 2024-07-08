#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

string longestSubstringWithKUnique(string s, int k) {
    int n = s.length();
    if (n * k == 0) return "";

    int left = 0, right = 0;
    unordered_map<char, int> hashmap;
    int max_length = 0;
    int start_index = 0;

    while (right < n) {
        hashmap[s[right]]++;
        right++;

        while (hashmap.size() > k) {
            hashmap[s[left]]--;
            if (hashmap[s[left]] == 0) {
                hashmap.erase(s[left]);
            }
            left++;
        }

        if (right - left > max_length) {
            max_length = right - left;
            start_index = left;
        }
    }

    return s.substr(start_index, max_length);
}

int main() {
    string s = "araaci";
    int k = 2;
    cout << "Longest substring with " << k << " unique characters is " << longestSubstringWithKUnique(s, k) << endl;  // Output: "araa"
    return 0;
}
