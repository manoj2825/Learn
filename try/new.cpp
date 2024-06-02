#include <iostream>
#include <vector>

using namespace std;

int findUniqueXOR(const vector<int>& arr) {
    int unique = 0;
    for (int num : arr) {
        unique ^= num;
    }
    return unique;
}

int main() {
    vector<int> arr = {1, 5, 9, 3, 5, 7, 3, 5, 9};
    int uniqueNumber = findUniqueXOR(arr);

    cout << "The number that occurs only once is: " << uniqueNumber << endl;

    return 0;
}