#include <iostream>
#include <vector>
using namespace std;

int removeDuplicates(vector<int>& nums)
{
    if(nums.empty()) return 0;

    int uniqueindex=0;
    for(int i=1;i<nums.size();++i)
    {
        if(nums[i]!=nums[uniqueindex])
        {
            ++uniqueindex;
            nums[uniqueindex]=nums[i];
        }
    }
    //you can delete the rest of the duplicate numbers by resize with the new length like
    //nums.resize(uniqueindex+1);
    return uniqueindex+1;
}

int main() {
    std::vector<int> nums = {1, 1, 2, 2, 3, 4, 4, 5};

    int newLength = removeDuplicates(nums);

    std::cout << "New length: " << newLength << std::endl;
    std::cout << "Array after removing duplicates: ";

    for (int i = 0; i < newLength; ++i) {
        std::cout << nums[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}