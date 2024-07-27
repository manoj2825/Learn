#include <iostream>
using namespace std;

void func() {
    static int count = 0;
    count++;
    std::cout << count << std::endl;
}
// 

int main() {

    func();
    func();
    func();
    func();
}