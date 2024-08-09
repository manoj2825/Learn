#include <iostream>
using namespace std;

int main() {
    //read lamda theory before this
    int x=5;
    int y=10;
    auto add = [&]()->int
    {
        return x+y;
    };
    
    x=19;
    y=1;

    cout<<add()<<endl;
}