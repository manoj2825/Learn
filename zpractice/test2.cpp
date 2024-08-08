#include <iostream>
#include <vector>
int canMakeZeroArray(std::vector<int>& a) {
    for (int i = 1; i < a.size(); i++) {
        if (a[i] % a[0] != 0) {
            return 0; // If any element is not divisible by the first element, it's not possible.
        }
    }
    return 1; // If all elements are divisible by the first element, it's possible.
}



int canMakeAllZeros(std::vector<int>& a) {
    int n = a.size();
    if (n < 2) return a.front() == 0; // If array has less than 2 elements, check the only element.

    for (int i = n - 1; i > 0; --i) {
        if (a[i] < 0) return 0; // If any element is negative, it's immediately impossible.
        if (a[i-1] < a[i]) return 0; // If the preceding element can't be decremented enough times.
        a[i-1] -= a[i]; // Perform the decrement operation.
    }

    return a[0] == 0 ? 1:0;
}


int main() {
    // Example usage
    std::vector<int> a = {1, 2, 3}; // Example from the provided image


    std::cout << "Can the array be made all zeros? " << (canMakeZeroArray(a) ? "Yes" : "No") << std::endl;

        std::cout << "Can the array be made all zeros? " << (canMakeAllZeros(a) ? "Yes" : "No") << std::endl;
    
    return 0;
}
