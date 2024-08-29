#include <iostream>
using namespace std;

class Test
{
    public:
        static int var;

        void display()
        {
            cout<<var<<endl;
        }
};

int Test::var = 10;

int main() {
    
    Test t;
    t.display();
}