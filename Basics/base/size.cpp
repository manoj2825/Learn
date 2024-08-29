#include <iostream>
#include <climits>
#include <vector>

using namespace std;



class Empty {
    // No data members or member functions
    public:
        // int data;   //* * * * 
        // char ch;    //* * * *
        virtual void display()
        {

        }

};

int main() {
    std::cout << "Size of Empty class (type): " << sizeof(Empty) << " byte(s)" << std::endl;
    Empty e;
    std::cout << "Size of object e: " << sizeof(e) << " byte(s)" << std::endl;
    return 0;
}