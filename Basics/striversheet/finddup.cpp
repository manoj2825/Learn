#include <iostream>
#include <vector>
#include <climits>
using namespace std;

int solve(vector<int> &nums)
{
        // Find the intersection point of two runners
    int tortoise = 0;
    int hare = 0;
    do {
        tortoise = nums[tortoise];//{1,3,4,2,2}
        hare = nums[hare];
        hare = nums[hare];
    } while(tortoise != hare);
    // Find the "entrace" to the cycle (hare speed is 0.5x)
    tortoise = 0;
    do {
        tortoise = nums[tortoise];
        hare = nums[hare];
    } while(tortoise != hare);

    return tortoise;
   
}

int main() {
    
    // ios_base::sync_with_stdio(false);
    // cin.tie(NULL);
    vector<int> nums = {1,3,4,2,2};
    int n=nums.size();

    int duplicate =solve(nums);

    cout<<duplicate<<endl;

}